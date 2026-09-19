import { common } from '@kit.AbilityKit';
import { deviceInfo } from '@kit.BasicServicesKit';
import { image } from '@kit.ImageKit';
import { KeyCode, pointer } from '@kit.InputKit';
import { resourceManager } from '@kit.LocalizationKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';

const DOMAIN = 0x5346;
const TAG = 'SFML-Harmony';

const SYSTEM_POINTER_STYLES: Array<pointer.PointerStyle> = [
  pointer.PointerStyle.DEFAULT,
  pointer.PointerStyle.RUNNING,
  pointer.PointerStyle.LOADING,
  pointer.PointerStyle.TEXT_CURSOR,
  pointer.PointerStyle.HAND_POINTING,
  pointer.PointerStyle.RESIZE_LEFT_RIGHT,
  pointer.PointerStyle.RESIZE_UP_DOWN,
  pointer.PointerStyle.NORTH_WEST_SOUTH_EAST,
  pointer.PointerStyle.NORTH_EAST_SOUTH_WEST,
  pointer.PointerStyle.WEST,
  pointer.PointerStyle.EAST,
  pointer.PointerStyle.NORTH,
  pointer.PointerStyle.SOUTH,
  pointer.PointerStyle.NORTH_WEST,
  pointer.PointerStyle.SOUTH_EAST,
  pointer.PointerStyle.SOUTH_WEST,
  pointer.PointerStyle.NORTH_EAST,
  pointer.PointerStyle.MOVE,
  pointer.PointerStyle.CROSS,
  pointer.PointerStyle.HELP,
  pointer.PointerStyle.CURSOR_FORBID
];

class SfmlHostCallbacks {
  setPointerVisible: (visible: boolean) => void;
  setSystemPointer: (type: number) => void;
  setCustomPointer: (
    pixels: Uint8Array,
    width: number,
    height: number,
    hotspotX: number,
    hotspotY: number
  ) => void;
  setVirtualKeyboardVisible: (visible: boolean) => void;
  configureWindow: (fullscreen: boolean, keepScreenOn: boolean) => void;
  requestExit: () => void;

  constructor(
    setPointerVisible: (visible: boolean) => void,
    setSystemPointer: (type: number) => void,
    setCustomPointer: (
      pixels: Uint8Array,
      width: number,
      height: number,
      hotspotX: number,
      hotspotY: number
    ) => void,
    setVirtualKeyboardVisible: (visible: boolean) => void,
    configureWindow: (fullscreen: boolean, keepScreenOn: boolean) => void,
    requestExit: () => void
  ) {
    this.setPointerVisible = setPointerVisible;
    this.setSystemPointer = setSystemPointer;
    this.setCustomPointer = setCustomPointer;
    this.setVirtualKeyboardVisible = setVirtualKeyboardVisible;
    this.configureWindow = configureWindow;
    this.requestExit = requestExit;
  }
}

interface SfmlNativeContext {
  initializeHost(
    manager: resourceManager.ResourceManager,
    windowId: number,
    deviceType: string,
    callbacks: SfmlHostCallbacks
  ): boolean;
  onForeground(): void;
  onBackground(): void;
  onXComponentDestroy(): void;
  onDestroy(): void;
  submitText(text: string): void;
  submitKeyText(text: string): void;
  submitTextEdit(backwardDeletions: number, forwardDeletions: number, text: string): boolean;
  surfaceFallbackAck(): number;
  surfaceRestoreAck(): number;
  surfaceStressFailures(): number;
}

class SurfaceNodeBinding {
  context?: SfmlNativeContext;
  registered: boolean = false;
  retired: boolean = false;
  destroyed: boolean = false;
}

@Component
struct SfmlSurfaceNode {
  private binding: SurfaceNodeBinding = new SurfaceNodeBinding();
  nodeAppeared: (binding: SurfaceNodeBinding) => void = (_binding: SurfaceNodeBinding): void => {};
  surfaceLoaded: (binding: SurfaceNodeBinding) => void = (_binding: SurfaceNodeBinding): void => {};
  surfaceDestroyed: (binding: SurfaceNodeBinding) => void = (_binding: SurfaceNodeBinding): void => {};
  surfaceKeyEvent: (binding: SurfaceNodeBinding, event: KeyEvent) => void = (
    _binding: SurfaceNodeBinding,
    _event: KeyEvent
  ): void => {};

  aboutToAppear(): void {
    if (!this.binding.registered) {
      this.binding.registered = true;
      this.nodeAppeared(this.binding);
    }
  }

  private handleLoad(context?: object): void {
    if (!context) {
      hilog.error(DOMAIN, TAG, 'XComponent onLoad did not provide a native context');
      return;
    }

    const nextContext = context as SfmlNativeContext;
    if (this.binding.destroyed || this.binding.retired) {
      hilog.info(DOMAIN, TAG, 'Retiring XComponent load after its node was retired');
      nextContext.onXComponentDestroy();
      return;
    }

    if (this.binding.context === nextContext) {
      hilog.info(DOMAIN, TAG, 'Ignoring duplicate XComponent load callback');
      return;
    }
    this.binding.context?.onXComponentDestroy();
    this.binding.context = nextContext;
    this.surfaceLoaded(this.binding);
  }

  private handleDestroy(): void {
    if (this.binding.destroyed) {
      hilog.info(DOMAIN, TAG, 'Ignoring duplicate XComponent destroy callback');
      return;
    }
    this.binding.destroyed = true;
    this.surfaceDestroyed(this.binding);
  }

  build() {
    XComponent({
      id: 'sfml-surface',
      type: XComponentType.SURFACE,
      libraryname: 'entry'
    })
      .width('100%')
      .height('100%')
      .focusable(true)
      .defaultFocus(true)
      .onLoad((context?: object) => this.handleLoad(context))
      .onKeyEvent((event: KeyEvent) => this.surfaceKeyEvent(this.binding, event))
      .onDestroy(() => this.handleDestroy());
  }
}

class ImeEditDelta {
  backwardDeletions: number;
  forwardDeletions: number;
  insertedText: string;
  selectionStart: number;
  selectionEnd: number;

  constructor(
    backwardDeletions: number,
    forwardDeletions: number,
    insertedText: string,
    selectionStart: number,
    selectionEnd: number
  ) {
    this.backwardDeletions = backwardDeletions;
    this.forwardDeletions = forwardDeletions;
    this.insertedText = insertedText;
    this.selectionStart = selectionStart;
    this.selectionEnd = selectionEnd;
  }
}

@Entry
@Component
struct Index {
  @StorageLink('sfmlWindowId') @Watch('windowIdChanged') windowId: number = 0;
  @StorageLink('sfmlLifecycle') @Watch('lifecycleChanged') lifecycle: string = 'created';
  @State status: string = 'Waiting for the native surface';
  @State imeText: string = '';
  @State surfaceMounted: boolean = true;
  @State surfaceStressStatus: string = '';
  @State surfaceStressRunning: boolean = false;
  @State hostInitialized: boolean = false;
  private nativeContext?: SfmlNativeContext;
  private lifecycleContext?: SfmlNativeContext;
  private latestSurfaceBinding?: SurfaceNodeBinding;
  private activeSurfaceBinding?: SurfaceNodeBinding;
  private nativeHostDestroyed: boolean = false;
  private imeController: TextInputController = new TextInputController();
  private imeCommittedText: string = '';
  private imeSelectionStart: number = 0;
  private imeSelectionEnd: number = 0;
  private customPointer?: image.PixelMap;
  private customPointerRequest: number = 0;
  private exitRequested: boolean = false;
  private surfaceStressPhase: string = 'idle';
  private surfaceStressCompleted: number = 0;
  private surfaceStressFailed: number = 0;
  private surfaceStressTimer: number = 0;
  private surfaceStressFallbackAck: number = 0;
  private surfaceStressRestoreAck: number = 0;
  private surfaceStressNativeFailures: number = 0;
  private surfaceStressTimeoutGeneration: number = 0;

  aboutToDisappear(): void {
    // Page teardown is an intentional cancellation, not a failed XComponent
    // stress transition. Stop its timers before child destroy callbacks run.
    this.cancelSurfaceStress();
  }

  private logHostFailure(operation: string, error: Error): void {
    hilog.warn(DOMAIN, TAG, '%{public}s failed: %{public}s', operation, error.message);
  }

  private releasePixelMap(pixelMap?: image.PixelMap): void {
    if (!pixelMap) {
      return;
    }
    pixelMap.release().catch((error: Error) => {
      this.logHostFailure('Releasing the custom pointer', error);
    });
  }

  private releaseCustomPointer(): void {
    const previous = this.customPointer;
    this.customPointer = undefined;
    this.releasePixelMap(previous);
  }

  private setPointerVisible(visible: boolean): void {
    try {
      pointer.setPointerVisibleSync(visible);
    } catch (error) {
      this.logHostFailure('Changing pointer visibility', error as Error);
    }
  }

  private setSystemPointer(type: number): void {
    ++this.customPointerRequest;
    const style = SYSTEM_POINTER_STYLES[type] ?? pointer.PointerStyle.DEFAULT;
    try {
      pointer.setPointerStyleSync(this.windowId, style);
      this.releaseCustomPointer();
    } catch (error) {
      this.logHostFailure('Changing the system pointer', error as Error);
    }
  }

  private setCustomPointer(
    pixels: Uint8Array,
    width: number,
    height: number,
    hotspotX: number,
    hotspotY: number
  ): void {
    if (width <= 0 || height <= 0 || pixels.length !== width * height * 4) {
      hilog.warn(DOMAIN, TAG, 'Rejected an invalid custom pointer');
      return;
    }

    const request = ++this.customPointerRequest;
    const options: image.InitializationOptions = {
      size: { width: width, height: height },
      srcPixelFormat: image.PixelMapFormat.RGBA_8888,
      pixelFormat: image.PixelMapFormat.RGBA_8888,
      alphaType: image.AlphaType.UNPREMUL,
      editable: false
    };

    image.createPixelMap(pixels.buffer, options)
      .then((pixelMap: image.PixelMap) => {
        if (request !== this.customPointerRequest) {
          this.releasePixelMap(pixelMap);
          return;
        }

        try {
          pointer.setCustomCursorSync(this.windowId, pixelMap, hotspotX, hotspotY);
          const previous = this.customPointer;
          this.customPointer = pixelMap;
          this.releasePixelMap(previous);
        } catch (error) {
          this.releasePixelMap(pixelMap);
          this.logHostFailure('Changing the custom pointer', error as Error);
        }
      })
      .catch((error: Error) => {
        this.logHostFailure('Creating the custom pointer image', error);
      });
  }

  private setVirtualKeyboardVisible(visible: boolean): void {
    try {
      if (visible) {
        this.resetImeEditor();
        this.getUIContext().getFocusController().requestFocus('sfml-ime');
      } else {
        this.imeController.stopEditing();
        this.getUIContext().getFocusController().requestFocus('sfml-surface');
        this.resetImeEditor();
      }
    } catch (error) {
      this.logHostFailure('Changing virtual keyboard visibility', error as Error);
    }
  }

  private configureWindow(
    context: common.UIAbilityContext,
    fullscreen: boolean,
    keepScreenOn: boolean
  ): void {
    window.getLastWindow(context).then((mainWindow: window.Window) => {
      const systemBars: Array<'status' | 'navigation'> = fullscreen ? [] : ['status', 'navigation'];
      return Promise.all([
        mainWindow.setWindowLayoutFullScreen(fullscreen),
        mainWindow.setWindowSystemBarEnable(systemBars),
        mainWindow.setWindowKeepScreenOn(keepScreenOn)
      ]);
    }).catch((error: Error) => {
      this.logHostFailure('Configuring the Stage window', error);
    });
  }

  private requestExit(context: common.UIAbilityContext): void {
    if (this.exitRequested) {
      return;
    }
    this.exitRequested = true;
    context.terminateSelf().catch((error: Error) => {
      this.exitRequested = false;
      this.logHostFailure('Terminating the UIAbility', error);
    });
  }

  private createHostCallbacks(context: common.UIAbilityContext): SfmlHostCallbacks {
    return new SfmlHostCallbacks(
      (visible: boolean) => this.setPointerVisible(visible),
      (type: number) => this.setSystemPointer(type),
      (pixels: Uint8Array, width: number, height: number, hotspotX: number, hotspotY: number) =>
        this.setCustomPointer(pixels, width, height, hotspotX, hotspotY),
      (visible: boolean) => this.setVirtualKeyboardVisible(visible),
      (fullscreen: boolean, keepScreenOn: boolean) => this.configureWindow(context, fullscreen, keepScreenOn),
      () => this.requestExit(context)
    );
  }

  private windowIdChanged(): void {
    this.tryInitializeNative();
  }

  private lifecycleChanged(): void {
    if (this.lifecycle === 'destroyed' || this.lifecycle === 'surface-destroyed') {
      this.cancelSurfaceStress();
      this.destroyNativeHost();
      return;
    }
    const context = this.nativeContext ?? this.lifecycleContext;
    if (!context) {
      return;
    }

    if (this.lifecycle === 'foreground') {
      context.onForeground();
    } else if (this.lifecycle === 'background') {
      context.onBackground();
    }
  }

  private tryInitializeNative(): void {
    if (!this.nativeContext || this.hostInitialized) {
      return;
    }
    if (this.windowId <= 0) {
      this.status = 'Waiting for the Stage window ID';
      return;
    }

    const hostContext = this.getUIContext().getHostContext();
    if (!hostContext) {
      this.status = 'Unable to obtain the UIAbility context';
      return;
    }
    const abilityContext = hostContext as common.UIAbilityContext;
    const initialized = this.nativeContext.initializeHost(
      abilityContext.resourceManager,
      this.windowId,
      deviceInfo.deviceType,
      this.createHostCallbacks(abilityContext)
    );

    this.hostInitialized = initialized;
    this.status = initialized ? '' : 'Unsupported device or native host failure';
    if (this.lifecycle === 'foreground') {
      this.nativeContext.onForeground();
    }
    hilog.info(DOMAIN, TAG, '%{public}s', this.status);
  }

  private registerSurfaceNode(binding: SurfaceNodeBinding): void {
    // The binding belongs to the child component instance and never changes
    // when this parent component is rebuilt. It therefore identifies the
    // precise XComponent that will deliver the matching destroy callback.
    this.latestSurfaceBinding = binding;
  }

  private initializeNative(binding: SurfaceNodeBinding): boolean {
    const nextContext = binding.context;
    if (!nextContext) {
      hilog.error(DOMAIN, TAG, 'XComponent load binding has no native context');
      return false;
    }

    if (!this.surfaceMounted || binding !== this.latestSurfaceBinding || binding.destroyed || binding.retired ||
      this.nativeHostDestroyed || this.lifecycle === 'destroyed' || this.lifecycle === 'surface-destroyed') {
      hilog.warn(
        DOMAIN,
        TAG,
        'Retiring stale XComponent load (mounted=%{public}s destroyed=%{public}s)',
        this.surfaceMounted ? 'true' : 'false',
        binding.destroyed ? 'true' : 'false'
      );
      binding.context = undefined;
      binding.retired = true;
      nextContext.onXComponentDestroy();
      return false;
    }

    const previousBinding = this.activeSurfaceBinding;
    if (previousBinding && previousBinding !== binding) {
      const previousContext = previousBinding.context;
      previousBinding.context = undefined;
      previousBinding.retired = true;
      previousContext?.onXComponentDestroy();
    }

    this.activeSurfaceBinding = binding;
    this.nativeContext = nextContext;
    // Retain the most recent NAPI object solely for final UIAbility teardown.
    // A stress cycle intentionally has a short interval with no mounted
    // XComponent; destruction during that interval must still notify native.
    this.lifecycleContext = this.nativeContext;
    this.tryInitializeNative();
    return true;
  }

  private destroyNativeHost(fallbackContext?: SfmlNativeContext): void {
    if (this.nativeHostDestroyed) {
      return;
    }
    this.nativeHostDestroyed = true;
    const context = this.nativeContext ?? this.lifecycleContext ?? fallbackContext;
    this.nativeContext = undefined;
    this.lifecycleContext = undefined;
    this.latestSurfaceBinding = undefined;
    this.activeSurfaceBinding = undefined;
    this.hostInitialized = false;
    ++this.customPointerRequest;
    this.releaseCustomPointer();
    context?.onDestroy();
  }

  private handleSurfaceNodeDestroy(binding: SurfaceNodeBinding): void {
    const context = binding.context;
    binding.context = undefined;
    binding.retired = true;
    const wasActive = binding === this.activeSurfaceBinding;

    // XComponent teardown is a recoverable surface-host transition (for
    // example during an ArkUI node rebuild). Only UIAbility destruction sends
    // the final Closed event and joins sfmlMain.
    if (this.lifecycle === 'destroyed' || this.lifecycle === 'surface-destroyed') {
      this.destroyNativeHost(context);
      return;
    }

    context?.onXComponentDestroy();
    if (!wasActive) {
      hilog.info(DOMAIN, TAG, 'Retired stale XComponent destroy without affecting the active node');
      return;
    }

    this.nativeContext = undefined;
    this.activeSurfaceBinding = undefined;
    this.hostInitialized = false;
    ++this.customPointerRequest;
    this.releaseCustomPointer();
    this.status = 'Native surface destroyed';
    this.handleSurfaceStressDestroy();
  }

  private handleSurfaceNodeLoad(binding: SurfaceNodeBinding): void {
    if (this.initializeNative(binding)) {
      this.handleSurfaceStressLoad();
    }
  }

  private handleSurfaceKeyEvent(binding: SurfaceNodeBinding, event: KeyEvent): void {
    if (binding !== this.activeSurfaceBinding) {
      return;
    }
    if (event.type === KeyType.Down && event.keyCode === KeyCode.KEYCODE_F10) {
      this.startSurfaceStress();
      return;
    }
    if (event.type === KeyType.Down && event.keySource !== KeySource.JOYSTICK && event.keyText.length > 0) {
      this.nativeContext?.submitKeyText(event.keyText);
    }
  }

  private resetImeEditor(): void {
    // Update the shadow before the bound TextInput. If ArkUI reports the
    // programmatic clear through onChange, it therefore becomes a no-op rather
    // than a user deletion.
    this.imeCommittedText = '';
    this.imeSelectionStart = 0;
    this.imeSelectionEnd = 0;
    this.imeText = '';
  }

  private clampImeOffset(value: number | undefined, length: number, fallback: number): number {
    if (value === undefined || !Number.isFinite(value)) {
      return fallback;
    }
    return Math.max(0, Math.min(length, Math.trunc(value)));
  }

  private isHighSurrogate(value: number): boolean {
    return value >= 0xD800 && value <= 0xDBFF;
  }

  private isLowSurrogate(value: number): boolean {
    return value >= 0xDC00 && value <= 0xDFFF;
  }

  private isImeBoundary(text: string, offset: number): boolean {
    return offset <= 0 || offset >= text.length ||
      !this.isHighSurrogate(text.charCodeAt(offset - 1)) || !this.isLowSurrogate(text.charCodeAt(offset));
  }

  private countImeCodePoints(text: string): number {
    let count = 0;
    let offset = 0;
    while (offset < text.length) {
      const first = text.charCodeAt(offset++);
      if (this.isHighSurrogate(first) && offset < text.length && this.isLowSurrogate(text.charCodeAt(offset))) {
        ++offset;
      }
      ++count;
    }
    return count;
  }

  private classifyImeDeletion(start: number, end: number, count: number): ImeEditDelta {
    let backward = count;
    let forward = 0;
    if (count > 0 && this.imeSelectionStart === this.imeSelectionEnd &&
      start === this.imeSelectionEnd && end > this.imeSelectionEnd) {
      backward = 0;
      forward = count;
    }
    return new ImeEditDelta(backward, forward, '', start, start);
  }

  private deriveImeEdit(value: string, options?: TextChangeOptions): ImeEditDelta {
    const previous = this.imeCommittedText;
    let beforeStart = 0;
    let beforeEnd = 0;
    let afterStart = 0;
    let afterEnd = 0;
    let hasExactRanges = false;

    if (options !== undefined && options.oldContent === previous) {
      beforeStart = this.clampImeOffset(options.rangeBefore.start, previous.length, 0);
      beforeEnd = this.clampImeOffset(options.rangeBefore.end, previous.length, previous.length);
      afterStart = this.clampImeOffset(options.rangeAfter.start, value.length, 0);
      afterEnd = this.clampImeOffset(options.rangeAfter.end, value.length, value.length);
      hasExactRanges = beforeStart <= beforeEnd && afterStart <= afterEnd &&
        this.isImeBoundary(previous, beforeStart) && this.isImeBoundary(previous, beforeEnd) &&
        this.isImeBoundary(value, afterStart) && this.isImeBoundary(value, afterEnd) &&
        previous.substring(0, beforeStart) === value.substring(0, afterStart) &&
        previous.substring(beforeEnd) === value.substring(afterEnd);
    }

    if (!hasExactRanges) {
      while (beforeStart < previous.length && beforeStart < value.length &&
        previous.charCodeAt(beforeStart) === value.charCodeAt(beforeStart)) {
        ++beforeStart;
      }
      while (beforeStart > 0 &&
        (!this.isImeBoundary(previous, beforeStart) || !this.isImeBoundary(value, beforeStart))) {
        --beforeStart;
      }

      beforeEnd = previous.length;
      afterEnd = value.length;
      while (beforeEnd > beforeStart && afterEnd > beforeStart &&
        previous.charCodeAt(beforeEnd - 1) === value.charCodeAt(afterEnd - 1)) {
        --beforeEnd;
        --afterEnd;
      }
      while ((!this.isImeBoundary(previous, beforeEnd) || !this.isImeBoundary(value, afterEnd)) &&
        beforeEnd < previous.length && afterEnd < value.length) {
        ++beforeEnd;
        ++afterEnd;
      }
      afterStart = beforeStart;
    }

    const removed = previous.substring(beforeStart, beforeEnd);
    const inserted = value.substring(afterStart, afterEnd);
    const deletion = this.classifyImeDeletion(beforeStart, beforeEnd, this.countImeCodePoints(removed));
    const nextSelection = afterStart + inserted.length;
    return new ImeEditDelta(
      deletion.backwardDeletions,
      deletion.forwardDeletions,
      inserted,
      nextSelection,
      nextSelection
    );
  }

  private handleImeChange(value: string, options?: TextChangeOptions): void {
    const edit = this.deriveImeEdit(value, options);
    this.imeCommittedText = value;
    this.imeText = value;
    this.imeSelectionStart = edit.selectionStart;
    this.imeSelectionEnd = edit.selectionEnd;

    if (edit.backwardDeletions === 0 && edit.forwardDeletions === 0 && edit.insertedText.length === 0) {
      return;
    }

    const accepted = this.nativeContext?.submitTextEdit(
      edit.backwardDeletions,
      edit.forwardDeletions,
      edit.insertedText
    );
    hilog.debug(
      DOMAIN,
      TAG,
      'ArkTS IME fallback edit accepted=%{public}s back=%{public}d forward=%{public}d insert=%{public}d',
      accepted === true ? 'true' : 'false',
      edit.backwardDeletions,
      edit.forwardDeletions,
      this.countImeCodePoints(edit.insertedText)
    );
  }

  private clearSurfaceStressTimer(): void {
    // Invalidate callbacks even when ArkTS assigned timer id 0, which is also
    // the sentinel used below and therefore cannot be passed to clearTimeout.
    ++this.surfaceStressTimeoutGeneration;
    if (this.surfaceStressTimer !== 0) {
      clearTimeout(this.surfaceStressTimer);
      this.surfaceStressTimer = 0;
    }
  }

  private updateSurfaceStressStatus(): void {
    this.surfaceStressStatus = `surface stress ${this.surfaceStressCompleted}/100 failed ${this.surfaceStressFailed}`;
  }

  private cancelSurfaceStress(): void {
    this.clearSurfaceStressTimer();
    this.surfaceStressRunning = false;
    this.surfaceStressPhase = 'idle';
    this.surfaceStressStatus = '';
  }

  private finishSurfaceStress(reason: string): void {
    this.clearSurfaceStressTimer();
    this.surfaceStressRunning = false;
    this.surfaceStressPhase = 'idle';
    this.surfaceMounted = true;
    this.updateSurfaceStressStatus();
    const summary = `${this.surfaceStressStatus} (${reason})`;
    this.surfaceStressStatus = summary;
    if (this.surfaceStressFailed === 0 && this.surfaceStressCompleted === 100) {
      hilog.info(DOMAIN, TAG, '%{public}s', summary);
    } else {
      hilog.error(DOMAIN, TAG, '%{public}s', summary);
    }
    this.surfaceStressTimer = setTimeout(() => {
      if (!this.surfaceStressRunning && this.surfaceStressStatus === summary) {
        this.surfaceStressStatus = '';
        this.surfaceStressTimer = 0;
      }
    }, 10000);
  }

  private failSurfaceStress(reason: string): void {
    ++this.surfaceStressFailed;
    this.finishSurfaceStress(reason);
  }

  private armSurfaceStressTimeout(phase: string): void {
    this.clearSurfaceStressTimer();
    const generation = this.surfaceStressTimeoutGeneration;
    this.surfaceStressTimer = setTimeout(() => {
      if (this.surfaceStressTimeoutGeneration === generation && this.surfaceStressRunning &&
        this.surfaceStressPhase === phase) {
        this.failSurfaceStress(`${phase} callback timeout`);
      }
    }, 5000);
  }

  private getSurfaceStressContext(): SfmlNativeContext | undefined {
    return this.nativeContext ?? this.lifecycleContext;
  }

  private checkSurfaceStressNativeFailures(context: SfmlNativeContext): boolean {
    const failures = context.surfaceStressFailures();
    if (failures === this.surfaceStressNativeFailures) {
      return true;
    }

    const delta = (failures - this.surfaceStressNativeFailures) >>> 0;
    this.failSurfaceStress(`native diagnostics reported ${delta} failure(s)`);
    return false;
  }

  private pollSurfaceStressFallback(deadline: number): void {
    if (!this.surfaceStressRunning || this.surfaceStressPhase !== 'fallback') {
      return;
    }

    const context = this.getSurfaceStressContext();
    if (!context) {
      this.failSurfaceStress('native context unavailable while awaiting fallback acknowledgement');
      return;
    }
    if (!this.checkSurfaceStressNativeFailures(context)) {
      return;
    }

    const acknowledgement = context.surfaceFallbackAck();
    const expected = (this.surfaceStressFallbackAck + 1) >>> 0;
    if (acknowledgement === expected) {
      if (!this.checkSurfaceStressNativeFailures(context)) {
        return;
      }
      this.surfaceStressFallbackAck = acknowledgement;
      // Snapshot before mounting the replacement node, so an immediate native
      // restore acknowledgement cannot be mistaken for an earlier cycle.
      this.surfaceStressRestoreAck = context.surfaceRestoreAck();
      this.surfaceStressPhase = 'load';
      this.surfaceMounted = true;
      this.armSurfaceStressTimeout('load');
      return;
    }
    if (acknowledgement !== this.surfaceStressFallbackAck) {
      this.failSurfaceStress(
        `fallback acknowledgement jumped from ${this.surfaceStressFallbackAck} to ${acknowledgement}`
      );
      return;
    }

    if (Date.now() >= deadline) {
      this.failSurfaceStress('fallback native acknowledgement timeout');
      return;
    }
    this.surfaceStressTimer = setTimeout(() => {
      this.surfaceStressTimer = 0;
      this.pollSurfaceStressFallback(deadline);
    }, 10);
  }

  private completeSurfaceStressCycle(): void {
    ++this.surfaceStressCompleted;
    this.updateSurfaceStressStatus();
    hilog.info(DOMAIN, TAG, 'Surface stress cycle %{public}d/100 restored', this.surfaceStressCompleted);
    if (this.surfaceStressCompleted === 100) {
      this.finishSurfaceStress('completed');
      return;
    }

    // Keep the rebuilt node alive for more than one ArkUI frame. The next
    // removal is scheduled only after native rendering acknowledges restore.
    this.surfaceStressPhase = 'settle';
    this.surfaceStressTimer = setTimeout(() => {
      this.surfaceStressTimer = 0;
      this.requestSurfaceStressDestroy();
    }, 100);
  }

  private pollSurfaceStressRestore(deadline: number): void {
    if (!this.surfaceStressRunning || this.surfaceStressPhase !== 'restore') {
      return;
    }

    const context = this.getSurfaceStressContext();
    if (!context) {
      this.failSurfaceStress('native context unavailable while awaiting restore acknowledgement');
      return;
    }
    if (!this.checkSurfaceStressNativeFailures(context)) {
      return;
    }

    const acknowledgement = context.surfaceRestoreAck();
    const expected = (this.surfaceStressRestoreAck + 1) >>> 0;
    if (acknowledgement === expected) {
      if (!this.checkSurfaceStressNativeFailures(context)) {
        return;
      }
      this.surfaceStressRestoreAck = acknowledgement;
      this.completeSurfaceStressCycle();
      return;
    }
    if (acknowledgement !== this.surfaceStressRestoreAck) {
      this.failSurfaceStress(
        `restore acknowledgement jumped from ${this.surfaceStressRestoreAck} to ${acknowledgement}`
      );
      return;
    }

    if (Date.now() >= deadline) {
      this.failSurfaceStress('restore native acknowledgement timeout');
      return;
    }
    this.surfaceStressTimer = setTimeout(() => {
      this.surfaceStressTimer = 0;
      this.pollSurfaceStressRestore(deadline);
    }, 10);
  }

  private requestSurfaceStressDestroy(): void {
    if (!this.surfaceStressRunning) {
      return;
    }
    const context = this.getSurfaceStressContext();
    if (!context) {
      this.failSurfaceStress('native context unavailable before surface destroy');
      return;
    }
    if (!this.checkSurfaceStressNativeFailures(context)) {
      return;
    }
    // Capture the native state before ArkUI can deliver the matching destroy
    // callback and the render thread can acknowledge its fallback context.
    this.surfaceStressFallbackAck = context.surfaceFallbackAck();
    this.surfaceStressPhase = 'destroy';
    this.updateSurfaceStressStatus();
    this.surfaceMounted = false;
    this.armSurfaceStressTimeout('destroy');
  }

  private handleSurfaceStressDestroy(): void {
    if (!this.surfaceStressRunning) {
      return;
    }
    if (this.surfaceStressPhase !== 'destroy') {
      this.failSurfaceStress(`unexpected destroy during ${this.surfaceStressPhase}`);
      return;
    }

    this.clearSurfaceStressTimer();
    this.surfaceStressPhase = 'fallback';
    this.pollSurfaceStressFallback(Date.now() + 5000);
  }

  private handleSurfaceStressLoad(): void {
    if (!this.surfaceStressRunning) {
      return;
    }
    if (this.surfaceStressPhase !== 'load') {
      this.failSurfaceStress(`unexpected load during ${this.surfaceStressPhase}`);
      return;
    }

    this.clearSurfaceStressTimer();
    if (!this.hostInitialized) {
      this.failSurfaceStress('native host initialization failed after load');
      return;
    }
    this.surfaceStressPhase = 'restore';
    this.pollSurfaceStressRestore(Date.now() + 5000);
  }

  private startSurfaceStress(): void {
    if (this.surfaceStressRunning) {
      return;
    }
    if (!this.surfaceMounted || !this.nativeContext || !this.hostInitialized) {
      hilog.warn(DOMAIN, TAG, 'Surface stress requires an initialized native surface');
      return;
    }

    this.clearSurfaceStressTimer();
    this.surfaceStressCompleted = 0;
    this.surfaceStressFailed = 0;
    this.surfaceStressFallbackAck = this.nativeContext.surfaceFallbackAck();
    this.surfaceStressRestoreAck = this.nativeContext.surfaceRestoreAck();
    this.surfaceStressNativeFailures = this.nativeContext.surfaceStressFailures();
    this.surfaceStressRunning = true;
    hilog.info(DOMAIN, TAG, 'Starting 100-cycle native-acknowledged XComponent surface stress');
    this.requestSurfaceStressDestroy();
  }

  build() {
    Stack({ alignContent: Alignment.TopStart }) {
      if (this.surfaceMounted) {
        SfmlSurfaceNode({
          nodeAppeared: (binding: SurfaceNodeBinding) => this.registerSurfaceNode(binding),
          surfaceLoaded: (binding: SurfaceNodeBinding) => this.handleSurfaceNodeLoad(binding),
          surfaceDestroyed: (binding: SurfaceNodeBinding) => this.handleSurfaceNodeDestroy(binding),
          surfaceKeyEvent: (binding: SurfaceNodeBinding, event: KeyEvent) => {
            this.handleSurfaceKeyEvent(binding, event);
          }
        })
          .width('100%')
          .height('100%');
      }

      TextInput({ text: this.imeText, controller: this.imeController })
        .id('sfml-ime')
        .width(1)
        .height(1)
        .opacity(0.01)
        .focusable(true)
        .maxLength(8192)
        .onChange((value: string, _previewText?: PreviewText, options?: TextChangeOptions) => {
          this.handleImeChange(value, options);
        })
        .onTextSelectionChange((selectionStart: number, selectionEnd: number) => {
          this.imeSelectionStart = selectionStart;
          this.imeSelectionEnd = selectionEnd;
        })
        .onSubmit(() => {
          this.nativeContext?.submitTextEdit(0, 0, '\r');
        });

      if (this.status.length > 0) {
        Text(this.status)
          .fontSize(12)
          .fontColor('#D1D5DB')
          .backgroundColor('#80000000')
          .padding(6)
          .margin(8)
          .hitTestBehavior(HitTestMode.None);
      }

      if (this.surfaceStressStatus.length > 0 && !this.surfaceMounted) {
        Text(this.surfaceStressStatus)
          .fontSize(12)
          .fontColor('#FFFFFF')
          .backgroundColor('#B0000000')
          .padding(6)
          .margin({ top: 40, left: 8 })
          .hitTestBehavior(HitTestMode.None);
      }

      if (!this.surfaceStressRunning && this.surfaceMounted && this.hostInitialized) {
        Row() {
          Button('surface ×100', { type: ButtonType.Capsule })
            .id('surface-stress')
            .height(24)
            .fontSize(10)
            .fontColor('#FFFFFF')
            .backgroundColor('#B0334154')
            .padding({ left: 10, right: 10 })
            .onClick(() => this.startSurfaceStress());
        }
        .width('100%')
        .height(32)
        .justifyContent(FlexAlign.Center)
        // Only the small button owns a hit target. The transparent row lets
        // every point outside it continue to the native XComponent.
        .hitTestBehavior(HitTestMode.None);
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#111827');
  }
}
