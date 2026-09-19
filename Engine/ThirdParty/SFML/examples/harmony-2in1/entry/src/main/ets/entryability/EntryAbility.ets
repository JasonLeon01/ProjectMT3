import {
  AbilityConstant,
  PermissionRequestResult,
  Permissions,
  UIAbility,
  Want,
  abilityAccessCtrl,
  bundleManager
} from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';

const DOMAIN = 0x5346;
const TAG = 'SFML-Harmony';

export default class EntryAbility extends UIAbility {
  private configureSupportedWindowModes(windowStage: window.WindowStage): Promise<void> {
    return windowStage.setSupportedWindowModes([
      bundleManager.SupportWindowMode.FULL_SCREEN,
      bundleManager.SupportWindowMode.FLOATING
    ]).then(() => {
      hilog.info(DOMAIN, TAG, 'Enabled full-screen and floating window modes');
    }).catch((error: Error) => {
      hilog.warn(DOMAIN, TAG, 'Unable to set supported window modes: %{public}s', error.message);
    });
  }

  private requestDiagnosticPermissions(): void {
    const permissions: Array<Permissions> = [
      'ohos.permission.MICROPHONE',
      'ohos.permission.READ_PASTEBOARD',
      'ohos.permission.ACCELEROMETER',
      'ohos.permission.GYROSCOPE'
    ];

    abilityAccessCtrl.createAtManager().requestPermissionsFromUser(this.context, permissions)
      .then((result: PermissionRequestResult) => {
        const denied: Array<string> = [];
        for (let index = 0; index < result.permissions.length; ++index) {
          if (result.authResults[index] !== 0) {
            denied.push(result.permissions[index]);
          }
        }

        if (denied.length > 0) {
          hilog.warn(DOMAIN, TAG, 'Diagnostics permission denied: %{public}s', denied.join(', '));
        } else {
          hilog.info(DOMAIN, TAG, 'Diagnostics permissions granted');
        }
      })
      .catch((error: Error) => {
        hilog.warn(DOMAIN, TAG, 'Unable to request diagnostics permissions: %{public}s', error.message);
      });
  }

  onCreate(_want: Want, _launchParam: AbilityConstant.LaunchParam): void {
    AppStorage.setOrCreate<number>('sfmlWindowId', 0);
    AppStorage.setOrCreate<string>('sfmlLifecycle', 'created');
    hilog.info(DOMAIN, TAG, 'EntryAbility created');
  }

  onDestroy(): void {
    AppStorage.set<string>('sfmlLifecycle', 'destroyed');
    hilog.info(DOMAIN, TAG, 'EntryAbility destroyed');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    this.requestDiagnosticPermissions();

    this.configureSupportedWindowModes(windowStage).then(() => {
      return windowStage.getMainWindow().then((mainWindow: window.Window) => {
        AppStorage.set<number>('sfmlWindowId', mainWindow.getWindowProperties().id);
      }).catch((error: Error) => {
        hilog.error(DOMAIN, TAG, 'Unable to obtain main window: %{public}s', error.message);
      });
    }).then(() => {
      windowStage.loadContent('pages/Index', (error) => {
        if (error.code) {
          hilog.error(DOMAIN, TAG, 'Unable to load page: %{public}s', JSON.stringify(error));
          return;
        }

        hilog.info(DOMAIN, TAG, 'SFML XComponent page loaded');
      });
    });
  }

  onWindowStageDestroy(): void {
    AppStorage.set<string>('sfmlLifecycle', 'surface-destroyed');
  }

  onForeground(): void {
    AppStorage.set<string>('sfmlLifecycle', 'foreground');
  }

  onBackground(): void {
    AppStorage.set<string>('sfmlLifecycle', 'background');
  }
}
