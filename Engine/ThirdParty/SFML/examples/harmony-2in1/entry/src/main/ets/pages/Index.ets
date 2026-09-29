import { common } from '@kit.AbilityKit';
import { deviceInfo } from '@kit.BasicServicesKit';
import { image } from '@kit.ImageKit';
import { KeyCode, pointer } from '@kit.InputKit';
import { resourceManager } from '@kit.LocalizationKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';

const DOMAIN = 0x5346;
const TAG = 'SFML-Harmony';

enum SfmlWindowCommandType {
  Configure = 0,
  Restore = 1,
  SetPosition = 2,
  SetSize = 3,
  SetMinimumSize = 4,
  SetMaximumSize = 5,
  SetTitle = 6,
  SetIcon = 7,
  SetVisible = 8,
  RequestFocus = 9
}

class SfmlWindowCommand {
  requestId: number = 0;
  timeoutMs: number = 0;
  type: SfmlWindowCommandType = SfmlWindowCommandType.Configure;
  x: number = 0;
  y: number = 0;
  width: number = 0;
  height: number = 0;
  hasSizeLimit: boolean = false;
  limitWidth: number = 0;
  limitHeight: number = 0;
  title: string = '';
  iconWidth: number = 0;
  iconHeight: number = 0;
  iconPixels: Uint8Array = new Uint8Array();
  style: number = 0;
  fullscreen: boolean = false;
  visible: boolean = true;
  keepScreenOn: boolean = false;
  expiresAt: number = 0;
}

class SfmlWindowState {
  x: number;
  y: number;
  width: number;
  height: number;
  visible: boolean;
  focused: boolean;
  fullscreen: boolean;

  constructor(
    x: number,
    y: number,
    width: number,
    height: number,
    visible: boolean,
    focused: boolean,
    fullscreen: boolean
  ) {
    this.x = x;
    this.y = y;
    this.width = width;
    this.height = height;
    this.visible = visible;
    this.focused = focused;
    this.fullscreen = fullscreen;
  }
}

class SfmlWindowPosition {
  x: number;
  y: number;

  constructor(x: number, y: number) {
    this.x = x;
    this.y = y;
  }
}

class SfmlWindowBaseline {
  rect: window.Rect;
  limits: window.WindowLimits;
  decorVisible: boolean;

  constructor(rect: window.Rect, limits: window.WindowLimits, decorVisible: boolean) {
    this.rect = rect;
    this.limits = limits;
    this.decorVisible = decorVisible;
  }
}

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
  executeWindowCommand: (command: SfmlWindowCommand) => void;
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
    executeWindowCommand: (command: SfmlWindowCommand) => void,
    requestExit: () => void
  ) {
    this.setPointerVisible = setPointerVisible;
    this.setSystemPointer = setSystemPointer;
    this.setCustomPointer = setCustomPointer;
    this.setVirtualKeyboardVisible = setVirtualKeyboardVisible;
    this.configureWindow = configureWindow;
    this.executeWindowCommand = executeWindowCommand;
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
  completeWindowCommand(
    requestId: number,
    success: boolean,
    x: number,
    y: number,
    width: number,
    height: number,
    visible: boolean,
    focused: boolean,
    fullscreen: boolean
  ): boolean;
  isWindowCommandPending(requestId: number): boolean;
  updateWindowState(
    x: number,
    y: number,
    width: number,
    height: number,
    visible: boolean,
    focused: boolean,
    fullscreen: boolean
  ): boolean;
  submitText(text: string): void;
  submitKeyText(text: string): void;
  submitTextEdit(backwardDeletions: number, forwardDeletions: number, text: string): boolean;
  surfaceFallbackAck(): number;
  surfaceRestoreAck(): number;
  surfaceStressFailures(): number;
}

class SurfaceNodeBinding {
  context?: SfmlNativeContext;
  requestFocus?: () => void;
  registered: boolean = false;
  retired: boolean = false;
  destroyed: boolean = false;
}

@Component
struct SfmlSurfaceNode {
  private binding: SurfaceNodeBinding = new SurfaceNodeBinding();
  nodeAppeared: (binding: SurfaceNodeBinding) => void = (_binding: SurfaceNodeBinding): void => {};
  surfaceLoaded: (binding: SurfaceNodeBinding) => void = (_binding: SurfaceNodeBinding): void => {};
  surfaceLoadFailed: (binding: SurfaceNodeBinding) => void = (_binding: SurfaceNodeBinding): void => {};
  surfaceDestroyed: (binding: SurfaceNodeBinding) => void = (_binding: SurfaceNodeBinding): void => {};
  surfaceKeyEvent: (binding: SurfaceNodeBinding, event: KeyEvent) => void = (
    _binding: SurfaceNodeBinding,
    _event: KeyEvent
  ): void => {};

  aboutToAppear(): void {
    this.binding.requestFocus = (): void => {
      // The XComponent constructor ID belongs to the native surface. ArkUI
      // focus lookup uses the separate universal inspector ID below.
      this.getUIContext().getFocusController().requestFocus('sfml-surface-focus');
    };
    if (!this.binding.registered) {
      this.binding.registered = true;
      this.nodeAppeared(this.binding);
    }
  }

  private handleLoad(context?: object): void {
    if (!context) {
      this.surfaceLoadFailed(this.binding);
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
    this.binding.requestFocus = undefined;
    this.surfaceDestroyed(this.binding);
  }

  build() {
    XComponent({
      id: 'sfml-surface',
      type: XComponentType.SURFACE,
      libraryname: 'entry'
    })
      .id('sfml-surface-focus')
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
  private mainWindow?: window.Window;
  private windowBaseline?: SfmlWindowBaseline;
  private windowStateObserversRegistered: boolean = false;
  private windowFocused: boolean = false;
  private windowStateGeneration: number = 0;
  private windowStatePublishSuppressed: boolean = false;
  private windowStatePublishDirty: boolean = false;
  private confirmedWindowTitle: string = 'SFML Harmony 2in1';
  private confirmedWindowIcon?: SfmlWindowCommand;
  private confirmedWindowStyle: number = 0;
  private confirmedKeepScreenOn: boolean = false;
  private pendingFloatingPosition?: SfmlWindowPosition;
  private suspendPendingFloatingPosition: boolean = false;
  private windowCommandQueue: Promise<void> = Promise.resolve();
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
      this.getUIContext().getFocusController().requestFocus('sfml-surface-focus');
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
      this.bindWindowStateObservers(mainWindow);
      return this.setFullscreenPolicy(mainWindow, fullscreen, keepScreenOn);
    }).catch((error: Error) => {
      this.logHostFailure('Configuring the Stage window', error);
    });
  }

  private queryWindowState(mainWindow: window.Window): SfmlWindowState {
    const properties = mainWindow.getWindowProperties();
    const rect = properties.globalDisplayRect ?? properties.windowRect;
    const drawable = properties.drawableRect;
    const status = mainWindow.getWindowStatus();
    this.windowFocused = mainWindow.isFocused();
    return new SfmlWindowState(
      rect.left,
      rect.top,
      drawable.width,
      drawable.height,
      mainWindow.isWindowShowing() && status !== window.WindowStatusType.MINIMIZE,
      this.windowFocused,
      properties.isFullScreen || properties.isLayoutFullScreen ||
        status === window.WindowStatusType.FULL_SCREEN
    );
  }

  private publishWindowState(mainWindow: window.Window): void {
    if (this.windowStatePublishSuppressed) {
      this.windowStatePublishDirty = true;
      return;
    }

    const nativeContext = this.nativeContext ?? this.lifecycleContext;
    if (!nativeContext || this.nativeHostDestroyed) {
      return;
    }

    try {
      const state = this.queryWindowState(mainWindow);
      nativeContext.updateWindowState(
        state.x,
        state.y,
        state.width,
        state.height,
        state.visible,
        state.focused,
        state.fullscreen
      );
    } catch (error) {
      this.logHostFailure('Publishing Stage window state', error as Error);
    }
  }

  private bindWindowStateObservers(mainWindow: window.Window): void {
    if (this.mainWindow === mainWindow && this.windowStateObserversRegistered) {
      return;
    }
    this.unbindWindowStateObservers();
    this.mainWindow = mainWindow;

    try {
      mainWindow.on('rectChangeInGlobalDisplay', (_options: window.RectChangeOptions) => {
        ++this.windowStateGeneration;
        this.publishWindowState(mainWindow);
      });
      mainWindow.on('windowStatusDidChange', (status: window.WindowStatusType) => {
        ++this.windowStateGeneration;
        this.publishWindowState(mainWindow);
        if (status === window.WindowStatusType.FLOATING && this.pendingFloatingPosition &&
          !this.suspendPendingFloatingPosition) {
          this.applyPendingFloatingPosition(mainWindow).catch((error: Error) => {
            this.logHostFailure('Applying deferred floating window position', error);
          });
        }
      });
      mainWindow.on('windowEvent', (event: window.WindowEventType) => {
        ++this.windowStateGeneration;
        if (event === window.WindowEventType.WINDOW_ACTIVE) {
          this.windowFocused = true;
        } else if (event === window.WindowEventType.WINDOW_INACTIVE ||
          event === window.WindowEventType.WINDOW_HIDDEN ||
          event === window.WindowEventType.WINDOW_DESTROYED) {
          this.windowFocused = false;
        }
        this.publishWindowState(mainWindow);
      });
      this.windowStateObserversRegistered = true;
      this.publishWindowState(mainWindow);
    } catch (error) {
      this.logHostFailure('Registering Stage window state observers', error as Error);
    }
  }

  private unbindWindowStateObservers(): void {
    const mainWindow = this.mainWindow;
    this.mainWindow = undefined;
    if (!mainWindow) {
      this.windowStateObserversRegistered = false;
      return;
    }

    try {
      mainWindow.off('rectChangeInGlobalDisplay');
    } catch (error) {
      this.logHostFailure('Unregistering global window rect observer', error as Error);
    }
    try {
      mainWindow.off('windowStatusDidChange');
    } catch (error) {
      this.logHostFailure('Unregistering window status observer', error as Error);
    }
    try {
      mainWindow.off('windowEvent');
    } catch (error) {
      this.logHostFailure('Unregistering window event observer', error as Error);
    }
    this.windowStateObserversRegistered = false;
  }

  private captureWindowBaseline(mainWindow: window.Window): void {
    if (this.windowBaseline) {
      return;
    }

    const properties = mainWindow.getWindowProperties();
    const currentRect = properties.globalDisplayRect ?? properties.windowRect;
    const rect: window.Rect = {
      left: currentRect.left,
      top: currentRect.top,
      width: currentRect.width,
      height: currentRect.height
    };
    const currentLimits = mainWindow.getWindowLimits();
    const limits: window.WindowLimits = {
      minWidth: currentLimits.minWidth,
      minHeight: currentLimits.minHeight,
      maxWidth: currentLimits.maxWidth,
      maxHeight: currentLimits.maxHeight
    };
    this.windowBaseline = new SfmlWindowBaseline(rect, limits, mainWindow.getWindowDecorVisible());
  }

  private cloneWindowBaseline(): SfmlWindowBaseline | undefined {
    const baseline = this.windowBaseline;
    if (!baseline) {
      return undefined;
    }

    const rect: window.Rect = {
      left: baseline.rect.left,
      top: baseline.rect.top,
      width: baseline.rect.width,
      height: baseline.rect.height
    };
    const limits: window.WindowLimits = {
      minWidth: baseline.limits.minWidth,
      minHeight: baseline.limits.minHeight,
      maxWidth: baseline.limits.maxWidth,
      maxHeight: baseline.limits.maxHeight
    };
    return new SfmlWindowBaseline(rect, limits, baseline.decorVisible);
  }

  private setFullscreenPolicy(
    mainWindow: window.Window,
    fullscreen: boolean,
    keepScreenOn: boolean
  ): Promise<void> {
    const systemBars: Array<'status' | 'navigation'> = fullscreen ? [] : ['status', 'navigation'];
    if (fullscreen) {
      mainWindow.setWindowDecorVisible(false);
      const alreadyMaximized = mainWindow.getWindowStatus() === window.WindowStatusType.MAXIMIZE ||
        mainWindow.getWindowStatus() === window.WindowStatusType.FULL_SCREEN;
      return Promise.all([
        mainWindow.setWindowLayoutFullScreen(true),
        mainWindow.setWindowSystemBarEnable(systemBars),
        mainWindow.setWindowKeepScreenOn(keepScreenOn)
      ]).then(() => alreadyMaximized ? Promise.resolve() :
        mainWindow.maximize(window.MaximizePresentation.ENTER_IMMERSIVE)).then((): void => {});
    }

    let recover: Promise<void> = Promise.resolve();
    if (mainWindow.getWindowStatus() === window.WindowStatusType.MINIMIZE) {
      recover = mainWindow.restore();
    }
    recover = recover.then(() => mainWindow.getWindowStatus() === window.WindowStatusType.FLOATING ?
      Promise.resolve() : mainWindow.recover());
    return recover.then(() => Promise.all([
      mainWindow.setWindowLayoutFullScreen(fullscreen),
      mainWindow.setWindowSystemBarEnable(systemBars),
      mainWindow.setWindowKeepScreenOn(keepScreenOn)
    ])).then((): void => {});
  }

  private applyPendingFloatingPosition(mainWindow: window.Window): Promise<void> {
    const pending = this.pendingFloatingPosition;
    if (!pending || mainWindow.getWindowStatus() !== window.WindowStatusType.FLOATING) {
      return Promise.resolve();
    }
    this.pendingFloatingPosition = undefined;
    return mainWindow.moveWindowToGlobalDisplay(pending.x, pending.y).catch((error: Error) => {
      this.pendingFloatingPosition = pending;
      throw error;
    });
  }

  private applyWindowStyle(mainWindow: window.Window, style: number): Promise<void> {
    const decorated = (style & 0x7) !== 0;
    const resizable = (style & 0x2) !== 0;
    const closeable = (style & 0x4) !== 0;
    mainWindow.setWindowDecorVisible(decorated);
    mainWindow.setWindowTitleMoveEnabled(decorated);
    mainWindow.setWindowTitleButtonVisible(resizable, decorated, closeable);
    return mainWindow.setResizeByDragEnabled(resizable);
  }

  private resizeClientArea(mainWindow: window.Window, width: number, height: number): Promise<void> {
    const properties = mainWindow.getWindowProperties();
    const horizontalDecor = Math.max(0, properties.windowRect.width - properties.drawableRect.width);
    const verticalDecor = Math.max(0, properties.windowRect.height - properties.drawableRect.height);
    return mainWindow.resize(
      Math.max(1, Math.trunc(width) + horizontalDecor),
      Math.max(1, Math.trunc(height) + verticalDecor)
    );
  }

  private setWindowSizeLimit(
    mainWindow: window.Window,
    command: SfmlWindowCommand,
    minimum: boolean
  ): Promise<window.WindowLimits> {
    const current = mainWindow.getWindowLimits();
    const limits: window.WindowLimits = {
      minWidth: current.minWidth,
      minHeight: current.minHeight,
      maxWidth: current.maxWidth,
      maxHeight: current.maxHeight
    };
    const properties = mainWindow.getWindowProperties();
    const horizontalDecor = Math.max(0, properties.windowRect.width - properties.drawableRect.width);
    const verticalDecor = Math.max(0, properties.windowRect.height - properties.drawableRect.height);
    const baseline = this.windowBaseline?.limits;

    if (minimum) {
      limits.minWidth = command.hasSizeLimit ? command.limitWidth + horizontalDecor : baseline?.minWidth;
      limits.minHeight = command.hasSizeLimit ? command.limitHeight + verticalDecor : baseline?.minHeight;
    } else {
      limits.maxWidth = command.hasSizeLimit ? command.limitWidth + horizontalDecor : baseline?.maxWidth;
      limits.maxHeight = command.hasSizeLimit ? command.limitHeight + verticalDecor : baseline?.maxHeight;
    }
    return mainWindow.setWindowLimits(limits);
  }

  private async setMissionWindowIcon(
    context: common.UIAbilityContext,
    command: SfmlWindowCommand
  ): Promise<void> {
    if (command.iconWidth <= 0 || command.iconHeight <= 0 ||
      command.iconPixels.length !== command.iconWidth * command.iconHeight * 4) {
      throw new Error('Invalid RGBA mission window icon payload');
    }

    const options: image.InitializationOptions = {
      size: { width: command.iconWidth, height: command.iconHeight },
      srcPixelFormat: image.PixelMapFormat.RGBA_8888,
      pixelFormat: image.PixelMapFormat.RGBA_8888,
      alphaType: image.AlphaType.UNPREMUL,
      editable: false
    };
    const pixelMap = await image.createPixelMap(command.iconPixels.buffer, options);
    try {
      await context.setMissionWindowIcon(pixelMap);
    } finally {
      await pixelMap.release();
    }
  }

  private async restoreWindowBaseline(mainWindow: window.Window): Promise<void> {
    const baseline = this.windowBaseline;
    this.suspendPendingFloatingPosition = true;
    try {
      await this.setFullscreenPolicy(mainWindow, false, false);
      if (mainWindow.getWindowStatus() === window.WindowStatusType.MINIMIZE) {
        await mainWindow.restore();
      }

      if (baseline) {
        mainWindow.setWindowDecorVisible(baseline.decorVisible);
        mainWindow.setWindowTitleMoveEnabled(baseline.decorVisible);
        mainWindow.setWindowTitleButtonVisible(true, true, true);
        await mainWindow.setResizeByDragEnabled(true);
        await mainWindow.setWindowLimits(baseline.limits);
        await mainWindow.resize(baseline.rect.width, baseline.rect.height);
        await mainWindow.moveWindowToGlobalDisplay(baseline.rect.left, baseline.rect.top);
      }
    } finally {
      this.suspendPendingFloatingPosition = false;
    }
    await this.applyPendingFloatingPosition(mainWindow);
  }

  private async applyWindowCommand(
    context: common.UIAbilityContext,
    mainWindow: window.Window,
    command: SfmlWindowCommand
  ): Promise<void> {
    switch (command.type) {
      case SfmlWindowCommandType.Configure:
        this.captureWindowBaseline(mainWindow);
        await mainWindow.setWindowTitle(command.title);
        if (command.fullscreen) {
          await this.applyWindowStyle(mainWindow, command.style);
          await this.setFullscreenPolicy(mainWindow, true, command.keepScreenOn);
        } else {
          this.suspendPendingFloatingPosition = true;
          try {
            await this.setFullscreenPolicy(mainWindow, false, command.keepScreenOn);
            await this.applyWindowStyle(mainWindow, command.style);
            if (command.width > 0 && command.height > 0) {
              await this.resizeClientArea(mainWindow, command.width, command.height);
            }
          } finally {
            this.suspendPendingFloatingPosition = false;
          }
          await this.applyPendingFloatingPosition(mainWindow);
        }
        if (command.visible) {
          await mainWindow.showWindow({ focusOnShow: true });
        }
        break;
      case SfmlWindowCommandType.Restore:
        await this.restoreWindowBaseline(mainWindow);
        break;
      case SfmlWindowCommandType.SetPosition:
        if (mainWindow.getWindowStatus() === window.WindowStatusType.FLOATING) {
          await mainWindow.moveWindowToGlobalDisplay(command.x, command.y);
        } else {
          this.pendingFloatingPosition = new SfmlWindowPosition(command.x, command.y);
        }
        break;
      case SfmlWindowCommandType.SetSize:
        await this.resizeClientArea(mainWindow, command.width, command.height);
        break;
      case SfmlWindowCommandType.SetMinimumSize:
        await this.setWindowSizeLimit(mainWindow, command, true);
        break;
      case SfmlWindowCommandType.SetMaximumSize:
        await this.setWindowSizeLimit(mainWindow, command, false);
        break;
      case SfmlWindowCommandType.SetTitle:
        await mainWindow.setWindowTitle(command.title);
        break;
      case SfmlWindowCommandType.SetIcon:
        await this.setMissionWindowIcon(context, command);
        break;
      case SfmlWindowCommandType.SetVisible:
        if (command.visible) {
          if (mainWindow.getWindowStatus() === window.WindowStatusType.MINIMIZE) {
            await mainWindow.restore();
          }
          await mainWindow.showWindow({ focusOnShow: false });
        } else {
          await mainWindow.minimize();
        }
        break;
      case SfmlWindowCommandType.RequestFocus:
        if (!mainWindow.isWindowShowing() ||
          mainWindow.getWindowStatus() === window.WindowStatusType.MINIMIZE) {
          throw new Error('Cannot focus a hidden or minimized Stage window');
        }
        await mainWindow.showWindow({ focusOnShow: true });
        let focusError: Error | undefined;
        for (let attempt = 0; attempt < 25 && Date.now() + 100 < command.expiresAt; ++attempt) {
          try {
            const requestSurfaceFocus = this.activeSurfaceBinding?.requestFocus;
            if (!requestSurfaceFocus) {
              throw new Error('The SFML XComponent focus target is not available');
            }
            requestSurfaceFocus();
            focusError = undefined;
            break;
          } catch (error) {
            focusError = error as Error;
            await this.delayWindowStatePoll();
          }
        }
        if (focusError) {
          if (!mainWindow.isFocused()) {
            throw focusError;
          }
          hilog.warn(
            DOMAIN,
            TAG,
            'requestFocus(sfml-surface) failed while the Stage window is focused; using the default-focus XComponent: %{public}s',
            focusError.message
          );
        }
        break;
      default:
        throw new Error(`Unknown SFML window command ${command.type}`);
    }
  }

  private windowStateSignature(mainWindow: window.Window, state: SfmlWindowState): string {
    return `${state.x},${state.y},${state.width},${state.height},${state.visible},${state.focused},` +
      `${state.fullscreen},${mainWindow.getWindowStatus()}`;
  }

  private windowCommandStateMatches(
    mainWindow: window.Window,
    command: SfmlWindowCommand,
    state: SfmlWindowState
  ): boolean {
    const status = mainWindow.getWindowStatus();
    switch (command.type) {
      case SfmlWindowCommandType.Configure:
        if (command.fullscreen) {
          return state.fullscreen && (status === window.WindowStatusType.MAXIMIZE ||
            status === window.WindowStatusType.FULL_SCREEN);
        }
        return status === window.WindowStatusType.FLOATING &&
          (command.width === 0 || state.width === command.width) &&
          (command.height === 0 || state.height === command.height);
      case SfmlWindowCommandType.Restore:
        return status === window.WindowStatusType.FLOATING;
      case SfmlWindowCommandType.SetPosition:
        return status !== window.WindowStatusType.FLOATING || (state.x === command.x && state.y === command.y);
      case SfmlWindowCommandType.SetSize:
        return state.width === command.width && state.height === command.height;
      case SfmlWindowCommandType.SetVisible:
        return state.visible === command.visible;
      case SfmlWindowCommandType.RequestFocus:
        return state.focused;
      default:
        return true;
    }
  }

  private delayWindowStatePoll(): Promise<void> {
    return new Promise<void>((resolve: () => void) => {
      setTimeout(() => resolve(), 20);
    });
  }

  private async awaitWindowCommandState(
    nativeContext: SfmlNativeContext,
    mainWindow: window.Window,
    command: SfmlWindowCommand,
    startingGeneration: number,
    startingState: SfmlWindowState
  ): Promise<void> {
    if (command.type === SfmlWindowCommandType.SetMinimumSize ||
      command.type === SfmlWindowCommandType.SetMaximumSize ||
      command.type === SfmlWindowCommandType.SetTitle || command.type === SfmlWindowCommandType.SetIcon) {
      return;
    }

    let previousSignature = '';
    let stablePolls = 0;
    for (let attempt = 0; attempt < 80; ++attempt) {
      if (Date.now() >= command.expiresAt || !nativeContext.isWindowCommandPending(command.requestId)) {
        throw new Error(`Window command ${command.requestId} expired while waiting for Stage state`);
      }
      const state = this.queryWindowState(mainWindow);
      const signature = this.windowStateSignature(mainWindow, state);
      stablePolls = signature === previousSignature ? stablePolls + 1 : 0;
      previousSignature = signature;
      const observedEvent = this.windowStateGeneration !== startingGeneration;

      if (this.windowCommandStateMatches(mainWindow, command, state) && (observedEvent || attempt >= 2)) {
        return;
      }
      const clampableGeometry = command.type === SfmlWindowCommandType.SetSize ||
        (command.type === SfmlWindowCommandType.SetPosition &&
          mainWindow.getWindowStatus() === window.WindowStatusType.FLOATING) ||
        (command.type === SfmlWindowCommandType.Configure && !command.fullscreen);
      const relevantGeometryChanged = command.type === SfmlWindowCommandType.SetPosition ?
        (state.x !== startingState.x || state.y !== startingState.y) :
        (state.width !== startingState.width || state.height !== startingState.height);
      // Only geometry can legitimately settle at a manager-clamped value.
      // Do not treat an unrelated delayed event as acknowledgement of an
      // unchanged drawable/position from the preceding serialized command.
      if (clampableGeometry && relevantGeometryChanged &&
        ((observedEvent && stablePolls >= 2) || (attempt >= 25 && stablePolls >= 5))) {
        return;
      }
      await this.delayWindowStatePoll();
    }
    throw new Error(`Stage state did not settle for window command ${command.requestId}`);
  }

  private completeHostWindowCommand(
    nativeContext: SfmlNativeContext,
    command: SfmlWindowCommand,
    success: boolean,
    mainWindow?: window.Window
  ): boolean {
    let state = new SfmlWindowState(0, 0, 0, 0, false, false, false);
    try {
      if (mainWindow) {
        state = this.queryWindowState(mainWindow);
      }
    } catch (error) {
      this.logHostFailure('Querying Stage state for command acknowledgement', error as Error);
    }
    return nativeContext.completeWindowCommand(
      command.requestId,
      success,
      state.x,
      state.y,
      state.width,
      state.height,
      state.visible,
      state.focused,
      state.fullscreen
    );
  }

  private async compensateFailedConfigure(mainWindow: window.Window, startingTitle: string): Promise<void> {
    try {
      await this.restoreWindowBaseline(mainWindow);
      await mainWindow.setWindowTitle(startingTitle);
      this.windowBaseline = undefined;
    } catch (error) {
      this.logHostFailure('Compensating a failed or expired Configure command', error as Error);
    }
  }

  private async compensateExpiredRestore(
    mainWindow: window.Window,
    startingState: SfmlWindowState,
    startingLimits: window.WindowLimits,
    startingPendingPosition: SfmlWindowPosition | undefined,
    startingBaseline: SfmlWindowBaseline | undefined,
    startingStyle: number,
    startingKeepScreenOn: boolean,
    startingDecorVisible: boolean
  ): Promise<void> {
    this.suspendPendingFloatingPosition = true;
    try {
      await mainWindow.setWindowLimits(startingLimits);
      if (startingState.fullscreen) {
        await this.applyWindowStyle(mainWindow, startingStyle);
        await this.setFullscreenPolicy(mainWindow, true, startingKeepScreenOn);
      } else {
        await this.setFullscreenPolicy(mainWindow, false, startingKeepScreenOn);
        await this.applyWindowStyle(mainWindow, startingStyle);
        await this.resizeClientArea(mainWindow, startingState.width, startingState.height);
        await mainWindow.moveWindowToGlobalDisplay(startingState.x, startingState.y);
      }

      mainWindow.setWindowDecorVisible(startingDecorVisible);
      mainWindow.setWindowTitleMoveEnabled((startingStyle & 0x7) !== 0);
      if (startingState.visible) {
        await mainWindow.showWindow({ focusOnShow: startingState.focused });
      } else if (mainWindow.getWindowStatus() !== window.WindowStatusType.MINIMIZE) {
        await mainWindow.minimize();
      }
    } finally {
      this.suspendPendingFloatingPosition = false;
      // Applying Restore may consume a deferred position, but a rejected
      // Restore must retain the exact Stage baseline for the next attempt.
      this.pendingFloatingPosition = startingPendingPosition;
      this.windowBaseline = startingBaseline;
    }
  }

  private async compensateExpiredWindowCommand(
    context: common.UIAbilityContext,
    mainWindow: window.Window,
    command: SfmlWindowCommand,
    startingState: SfmlWindowState,
    startingLimits: window.WindowLimits,
    startingTitle: string,
    startingIcon: SfmlWindowCommand | undefined,
    startingPendingPosition: SfmlWindowPosition | undefined,
    startingBaseline: SfmlWindowBaseline | undefined,
    startingStyle: number,
    startingKeepScreenOn: boolean,
    startingDecorVisible: boolean
  ): Promise<void> {
    if (this.nativeHostDestroyed) {
      return;
    }

    try {
      switch (command.type) {
        case SfmlWindowCommandType.Configure:
          await this.compensateFailedConfigure(mainWindow, startingTitle);
          break;
        case SfmlWindowCommandType.Restore:
          await this.compensateExpiredRestore(
            mainWindow,
            startingState,
            startingLimits,
            startingPendingPosition,
            startingBaseline,
            startingStyle,
            startingKeepScreenOn,
            startingDecorVisible
          );
          break;
        case SfmlWindowCommandType.SetPosition:
          this.pendingFloatingPosition = startingPendingPosition;
          if (mainWindow.getWindowStatus() === window.WindowStatusType.FLOATING) {
            await mainWindow.moveWindowToGlobalDisplay(startingState.x, startingState.y);
          }
          break;
        case SfmlWindowCommandType.SetSize:
          await this.resizeClientArea(mainWindow, startingState.width, startingState.height);
          break;
        case SfmlWindowCommandType.SetMinimumSize:
        case SfmlWindowCommandType.SetMaximumSize:
          await mainWindow.setWindowLimits(startingLimits);
          break;
        case SfmlWindowCommandType.SetTitle:
          await mainWindow.setWindowTitle(startingTitle);
          break;
        case SfmlWindowCommandType.SetIcon:
          if (startingIcon) {
            await this.setMissionWindowIcon(context, startingIcon);
          }
          break;
        case SfmlWindowCommandType.SetVisible:
          if (startingState.visible) {
            if (mainWindow.getWindowStatus() === window.WindowStatusType.MINIMIZE) {
              await mainWindow.restore();
            }
            await mainWindow.showWindow({ focusOnShow: startingState.focused });
          } else if (mainWindow.getWindowStatus() !== window.WindowStatusType.MINIMIZE) {
            await mainWindow.minimize();
          }
          break;
        case SfmlWindowCommandType.RequestFocus:
          // Harmony has no public "restore prior focus owner" operation. The
          // command is not started without the native deadline safety margin.
          break;
      }
    } catch (error) {
      this.logHostFailure(`Compensating expired window command ${command.requestId}`, error as Error);
    }
  }

  private async runHostWindowCommand(
    context: common.UIAbilityContext,
    nativeContext: SfmlNativeContext,
    command: SfmlWindowCommand
  ): Promise<void> {
    let mainWindow: window.Window | undefined;
    let startingState: SfmlWindowState | undefined;
    let startingLimits: window.WindowLimits | undefined;
    let startingDecorVisible = false;
    let statePublicationSuppressed = false;
    let mutationStarted = false;
    const startingTitle = this.confirmedWindowTitle;
    const startingIcon = this.confirmedWindowIcon;
    const startingStyle = this.confirmedWindowStyle;
    const startingKeepScreenOn = this.confirmedKeepScreenOn;
    const startingPendingPosition = this.pendingFloatingPosition ?
      new SfmlWindowPosition(this.pendingFloatingPosition.x, this.pendingFloatingPosition.y) : undefined;
    const startingBaseline = this.cloneWindowBaseline();
    if (Date.now() >= command.expiresAt || !nativeContext.isWindowCommandPending(command.requestId)) {
      if (nativeContext.isWindowCommandPending(command.requestId)) {
        this.completeHostWindowCommand(nativeContext, command, false);
      }
      return;
    }
    try {
      if (this.nativeHostDestroyed) {
        throw new Error('Native host was destroyed');
      }
      mainWindow = await window.getLastWindow(context);
      this.bindWindowStateObservers(mainWindow);
      if (Date.now() >= command.expiresAt || !nativeContext.isWindowCommandPending(command.requestId)) {
        if (nativeContext.isWindowCommandPending(command.requestId)) {
          this.completeHostWindowCommand(nativeContext, command, false, mainWindow);
        }
        return;
      }
      const startingGeneration = this.windowStateGeneration;
      startingState = this.queryWindowState(mainWindow);
      const currentLimits = mainWindow.getWindowLimits();
      startingLimits = {
        minWidth: currentLimits.minWidth,
        minHeight: currentLimits.minHeight,
        maxWidth: currentLimits.maxWidth,
        maxHeight: currentLimits.maxHeight
      };
      startingDecorVisible = mainWindow.getWindowDecorVisible();
      if (Date.now() + 100 >= command.expiresAt) {
        throw new Error(`Window command ${command.requestId} has insufficient time before its native deadline`);
      }
      this.windowStatePublishSuppressed = true;
      this.windowStatePublishDirty = false;
      statePublicationSuppressed = true;
      mutationStarted = true;
      await this.applyWindowCommand(context, mainWindow, command);
      const stillPending = nativeContext.isWindowCommandPending(command.requestId);
      if (Date.now() >= command.expiresAt || !stillPending) {
        if (startingState && startingLimits) {
          await this.compensateExpiredWindowCommand(
            context,
            mainWindow,
            command,
            startingState,
            startingLimits,
            startingTitle,
            startingIcon,
            startingPendingPosition,
            startingBaseline,
            startingStyle,
            startingKeepScreenOn,
            startingDecorVisible
          );
        }
        if (nativeContext.isWindowCommandPending(command.requestId)) {
          this.completeHostWindowCommand(nativeContext, command, false, mainWindow);
        }
        return;
      }
      await this.awaitWindowCommandState(nativeContext, mainWindow, command, startingGeneration, startingState);
      const accepted = this.completeHostWindowCommand(nativeContext, command, true, mainWindow);
      if (accepted) {
        if (command.type === SfmlWindowCommandType.Configure || command.type === SfmlWindowCommandType.SetTitle) {
          this.confirmedWindowTitle = command.title;
        }
        if (command.type === SfmlWindowCommandType.Configure) {
          this.confirmedWindowStyle = command.style;
          this.confirmedKeepScreenOn = command.keepScreenOn;
        } else if (command.type === SfmlWindowCommandType.SetIcon) {
          this.confirmedWindowIcon = command;
        } else if (command.type === SfmlWindowCommandType.Restore) {
          this.windowBaseline = undefined;
        }
      } else if (startingState && startingLimits) {
        await this.compensateExpiredWindowCommand(
          context,
          mainWindow,
          command,
          startingState,
          startingLimits,
          startingTitle,
          startingIcon,
          startingPendingPosition,
          startingBaseline,
          startingStyle,
          startingKeepScreenOn,
          startingDecorVisible
        );
      }
    } catch (error) {
      this.logHostFailure(`Executing window command ${command.requestId}`, error as Error);
      if (mutationStarted && mainWindow && startingState && startingLimits) {
        await this.compensateExpiredWindowCommand(
          context,
          mainWindow,
          command,
          startingState,
          startingLimits,
          startingTitle,
          startingIcon,
          startingPendingPosition,
          startingBaseline,
          startingStyle,
          startingKeepScreenOn,
          startingDecorVisible
        );
      }
      if (nativeContext.isWindowCommandPending(command.requestId)) {
        this.completeHostWindowCommand(nativeContext, command, false, mainWindow);
      }
    } finally {
      if (statePublicationSuppressed) {
        // Successful acknowledgements already carry the final queried state;
        // failed commands retain the native side's last confirmed snapshot.
        this.windowStatePublishSuppressed = false;
        this.windowStatePublishDirty = false;
      }
    }
  }

  private executeWindowCommand(context: common.UIAbilityContext, command: SfmlWindowCommand): void {
    const nativeContext = this.nativeContext ?? this.lifecycleContext;
    if (!nativeContext) {
      hilog.warn(DOMAIN, TAG, 'Unable to acknowledge window command %{public}d without a native context', command.requestId);
      return;
    }
    // Keep a compensation margin before the native three-second deadline.
    command.expiresAt = Date.now() + Math.max(0, command.timeoutMs - 500);
    this.windowCommandQueue = this.windowCommandQueue.then(() =>
      this.runHostWindowCommand(context, nativeContext, command)
    ).catch((error: Error) => {
      this.logHostFailure(`Running window command ${command.requestId}`, error);
      if (nativeContext.isWindowCommandPending(command.requestId)) {
        this.completeHostWindowCommand(nativeContext, command, false);
      }
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
      (command: SfmlWindowCommand) => this.executeWindowCommand(context, command),
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
      if (this.mainWindow) {
        this.publishWindowState(this.mainWindow);
      }
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
      if (this.mainWindow) {
        this.publishWindowState(this.mainWindow);
      }
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
    this.unbindWindowStateObservers();
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

  private handleSurfaceNodeLoadFailure(binding: SurfaceNodeBinding): void {
    if (binding !== this.latestSurfaceBinding || binding.destroyed || binding.retired) {
      hilog.info(DOMAIN, TAG, 'Ignoring missing native context from a retired XComponent node');
      return;
    }

    this.hostInitialized = false;
    this.status = 'Native module unavailable. Desktop OpenGL may be missing libGLv4.so; build/run the gles target ' +
      '(no automatic fallback).';
    hilog.error(
      DOMAIN,
      TAG,
      'XComponent onLoad context is undefined; the native module or desktop OpenGL runtime may be unavailable ' +
        '(missing libGLv4.so). Build and run entry@gles; no automatic GLES fallback was attempted.'
    );
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
          surfaceLoadFailed: (binding: SurfaceNodeBinding) => this.handleSurfaceNodeLoadFailure(binding),
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
