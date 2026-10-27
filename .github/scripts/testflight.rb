# frozen_string_literal: true
# API-only authentication: no Apple ID session, browser, 2FA or external review.
require 'json'
require 'spaceship'
require 'timeout'
require 'socket'
require 'openssl'

class AppleProcessingDeadlineExceeded < StandardError; end

def processing_time_left(deadline)
  remaining = deadline - Process.clock_gettime(Process::CLOCK_MONOTONIC)
  raise AppleProcessingDeadlineExceeded, 'Apple processing/group assignment exceeded 45 minutes' if remaining <= 0
  remaining
end

def apple_retry_reason(error)
  errors = []
  pending = [error]
  until pending.empty?
    current = pending.pop
    next if current.nil? || errors.include?(current)
    errors << current
    pending << current.cause
    pending << current.wrapped_exception if current.respond_to?(:wrapped_exception)
  end
  # Never turn a business rejection, authentication or TLS failure into a retry.
  return if errors.any? do |item|
    [AppleProcessingDeadlineExceeded, Spaceship::UnexpectedResponse,
     Spaceship::UnauthorizedAccessError, Spaceship::AccessForbiddenError,
     Spaceship::InvalidUserCredentialsError, Spaceship::ProgramLicenseAgreementUpdated,
     Faraday::SSLError, OpenSSL::SSL::SSLError].any? { |type| item.is_a?(type) }
  end
  statuses = errors.filter_map do |item|
    status = item.response_status if item.respond_to?(:response_status)
    if status.nil? && item.is_a?(Spaceship::InternalServerError)
      status = item.message[/\bServer error got (\d{3})\b/i, 1]
    end
    status.to_i if status
  end
  return if statuses.any? { |status| !(200...400).cover?(status) && ![408, 429, 500, 502, 503, 504].include?(status) }
  return 'timeout' if statuses.any? { |status| [408, 504].include?(status) } || errors.any? do |item|
    [Timeout::Error, Faraday::TimeoutError, Errno::ETIMEDOUT,
     Spaceship::GatewayTimeoutError].any? { |type| item.is_a?(type) }
  end
  return 'network interruption' if errors.any? do |item|
    [Faraday::ConnectionFailed, SocketError, EOFError, Errno::ECONNRESET, Errno::ECONNREFUSED,
     Errno::ECONNABORTED, Errno::EPIPE, Errno::EHOSTUNREACH, Errno::ENETUNREACH].any? { |type| item.is_a?(type) }
  end
  return 'temporary service failure' if statuses.any? { |status| [429, 500, 502, 503].include?(status) } || errors.any? do |item|
    [Spaceship::AppleTimeoutError, Spaceship::BadGatewayError, Spaceship::InternalServerError,
     Spaceship::TooManyRequestsError, Spaceship::ConnectAPI::APIClient::TooManyRequestsError].any? { |type| item.is_a?(type) }
  end
end

# These are CI-level attempts after fastlane's own bounded request retries.
def apple_request(label, deadline: nil)
  attempt = 0
  begin
    remaining = processing_time_left(deadline) if deadline
    attempt += 1
    puts "#{label}: attempt #{attempt}/3"
    if deadline
      Timeout.timeout(remaining, AppleProcessingDeadlineExceeded, 'Apple processing/group assignment exceeded 45 minutes') { yield }
    else
      yield
    end
  rescue StandardError => error
    reason = apple_retry_reason(error)
    warn "#{label}: #{reason || 'non-retryable failure'} (attempt #{attempt}/3): #{error.class}: #{error.message}"
    raise unless reason && attempt < 3
    delay = 15 * attempt
    raise if deadline && delay >= processing_time_left(deadline)
    warn "#{label}: retrying in #{delay}s"
    sleep delay
    retry
  end
end

def connect
  Spaceship::ConnectAPI.token = Spaceship::ConnectAPI::Token.create(
    key_id: ENV.fetch('APP_STORE_CONNECT_KEY_ID'),
    issuer_id: ENV.fetch('APP_STORE_CONNECT_ISSUER_ID'),
    filepath: ENV.fetch('ASC_KEY_PATH'), duration: 1200, in_house: false
  )
end

def next_build_number(versions)
  values = versions.map do |version|
    raise "Unsupported existing Apple build number: #{version}" unless version.match?(/\A\d{1,4}(?:\.\d{1,2}){0,2}\z/)
    parts = version.split('.').map(&:to_i)
    parts.fill(0, parts.length...3)
    parts[0] * 10_000 + parts[1] * 100 + parts[2]
  end
  value = [10_000, (values.max || 0) + 1].max
  raise 'Apple build number space exhausted' if value > 99_999_999
  "#{value / 10_000}.#{value / 100 % 100}.#{value % 100}"
end

def report(context, output, status)
  context['status'] = status
  url = context['app_id'] ? "https://appstoreconnect.apple.com/apps/#{context['app_id']}/testflight/ios" : 'https://appstoreconnect.apple.com/'
  info_path = File.join(output, 'build-info.json')
  info = JSON.parse(File.read(info_path))
  info['testflight'] = context.merge('url' => url)
  File.write(info_path, JSON.pretty_generate(info) + "\n")
  File.write(File.join(output, 'testflight.txt'), <<~TEXT)
    ProjectMT3 — TestFlight Internal Testing
    Version: #{context['version']}
    Build number: #{context['build_number'] || 'Not yet assigned'}
    Status: #{status}
    Test group: #{ENV.fetch('IOS_TESTFLIGHT_GROUP')}
    Project commit: #{ENV.fetch('PROJECT_SHA')}
    App Store Connect: #{url}

    Only the ready status confirms that Apple has successfully processed the build and it has been added to the selected internal test group.
    Internal testing has no public invitation code. Testers must be App Store Connect users with access to this app.
    An administrator must add them to the internal test group. Accept Apple's individual invitation in TestFlight on an iPhone.
    CI does not retain IPA files, submit builds for external beta review, or create public links.
  TEXT
end

def prepare(context)
  connect
  app = Spaceship::ConnectAPI::App.find(context.fetch('bundle_id'))
  raise 'Create the matching iOS app in App Store Connect before enabling CI' unless app
  groups = app.get_beta_groups.select { |group| group.name == ENV.fetch('IOS_TESTFLIGHT_GROUP') }
  raise 'Expected one existing internal TestFlight group' unless groups.length == 1 && groups.first.is_internal_group
  builds = Spaceship::ConnectAPI::Build.all(app_id: app.id, platform: 'IOS', limit: 200)
  # Uploads may not have become Builds yet; include every page and processing state.
  uploads = Spaceship::ConnectAPI.get_build_uploads(
    app_id: app.id, filter: { platform: 'IOS' }, limit: 200
  ).all_pages.flat_map(&:to_models)
  versions = builds.map(&:version) + uploads.map(&:cf_build_version)
  context.merge!(
    'app_id' => app.id, 'group_id' => groups.first.id,
    'build_number' => next_build_number(versions)
  )
end

def wait_for_internal_testing(context, output)
  deadline = Process.clock_gettime(Process::CLOCK_MONOTONIC) + 2700
  attached = false
  loop do
    apple_request('TestFlight processing query', deadline: deadline) do
      connect # Refresh the short-lived API token while Apple processes the upload.
      builds = Spaceship::ConnectAPI::Build.all(
        app_id: context.fetch('app_id'), version: context.fetch('version'),
        build_number: context.fetch('build_number'), platform: 'IOS', limit: 200
      )
      raise 'Ambiguous uploaded build' if builds.length > 1
      build = builds.first
      if build
        state = build.build_beta_detail&.internal_build_state
        raise "Apple rejected this build: #{build.processing_state}/#{state}" if %w[FAILED INVALID].include?(build.processing_state) ||
          %w[PROCESSING_EXCEPTION MISSING_EXPORT_COMPLIANCE IN_EXPORT_COMPLIANCE_REVIEW EXPIRED].include?(state) || build.expired
        if build.processing_state == 'VALID' && %w[READY_FOR_BETA_TESTING IN_BETA_TESTING].include?(state)
          app = Spaceship::ConnectAPI::App.find(context.fetch('bundle_id'))
          group = app.get_beta_groups.find { |item| item.id == context.fetch('group_id') && item.is_internal_group }
          raise 'Configured internal TestFlight group no longer exists' unless group
          unless attached
            build.add_beta_groups(beta_groups: [group]) unless group.fetch_builds.any? { |item| item.id == build.id }
            attached = true
          end
          if group.fetch_builds.any? { |item| item.id == build.id }
            context['build_id'] = build.id
            report(context, output, 'ready')
            return
          end
        end
        puts "Apple processing: #{build.processing_state}/#{state || 'pending'}"
      else
        puts 'Waiting for the uploaded build to appear in App Store Connect'
      end
    end
    sleep [30, processing_time_left(deadline)].min
  end
end

def uploaded?(context)
  connect
  builds = Spaceship::ConnectAPI::Build.all(
    app_id: context.fetch('app_id'), version: context.fetch('version'),
    build_number: context.fetch('build_number'), platform: 'IOS', limit: 200
  )
  return true unless builds.empty?
  # BuildUpload appears before Build while Apple is processing a received IPA.
  Spaceship::ConnectAPI.get_build_uploads(
    app_id: context.fetch('app_id'), filter: { platform: 'IOS' }, limit: 200
  ).all_pages.flat_map(&:to_models).any? { |item| item.cf_build_version == context.fetch('build_number') }
end

if $PROGRAM_NAME == __FILE__
  command, context_path, output = ARGV
  raise 'Usage: testflight.rb <prepare|wait|uploaded|failed> <context.json> <output>' unless %w[prepare wait uploaded failed].include?(command) && output
  context = JSON.parse(File.read(context_path))
  begin
    case command
    when 'prepare'
      apple_request('TestFlight preparation query') { prepare(context) }
      File.write(context_path, JSON.pretty_generate(context) + "\n")
      report(context, output, 'prepared')
    when 'wait'
      report(context, output, 'processing')
      wait_for_internal_testing(context, output)
    when 'uploaded'
      exit(apple_request('TestFlight upload query') { uploaded?(context) } ? 0 : 2)
    when 'failed'
      report(context, output, 'failed; see workflow logs')
    end
  rescue StandardError
    report(context, output, 'failed; see workflow logs')
    raise
  end
end
