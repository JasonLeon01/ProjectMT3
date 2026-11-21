#import <UIKit/UIKit.h>

#include <CoreFoundation/CoreFoundation.h>

void              waitForActiveScene();
[[noreturn]] void showTestResult(int result, const char* mode);

// NOLINTBEGIN(readability-identifier-naming)
@interface SFTestSceneDelegate : UIResponder<UIWindowSceneDelegate>

@property (nonatomic, strong) UIWindow* window;

@end
// NOLINTEND(readability-identifier-naming)

@implementation SFTestSceneDelegate

- (void)scene:(UIScene*)scene
    willConnectToSession:(UISceneSession*)session
                 options:(UISceneConnectionOptions*)connectionOptions
{
    if (![scene isKindOfClass:UIWindowScene.class])
        return;
    auto* const     windowScene                         = static_cast<UIWindowScene*>(scene);
    UIWindow* const placeholder                         = [[UIWindow alloc] initWithWindowScene:windowScene];
    placeholder.frame                                   = windowScene.coordinateSpace.bounds;
    placeholder.rootViewController                      = [[UIViewController alloc] init];
    placeholder.rootViewController.view.backgroundColor = UIColor.blackColor;
    self.window                                         = placeholder;
    [placeholder makeKeyAndVisible];

    // SFML creates its windows without a scene, which leaves them hidden in a scene based application
    __weak SFTestSceneDelegate* const owner = self;
    [[NSNotificationCenter defaultCenter]
        addObserverForName:UIWindowDidBecomeVisibleNotification
                    object:nil
                     queue:nil
                usingBlock:^(NSNotification* notification) {
                    UIWindow* const window = notification.object;
                    if (window.windowScene || !owner)
                        return;
                    window.windowScene = windowScene;
                    window.frame       = windowScene.coordinateSpace.bounds;
                    owner.window       = window;
                    [window makeKeyAndVisible];
                    [window layoutIfNeeded];
                    [window.rootViewController.view layoutIfNeeded];
                }];
}

- (void)sceneDidBecomeActive:(UIScene*)scene
{
    const id<UIApplicationDelegate> delegate = UIApplication.sharedApplication.delegate;
    if ([delegate respondsToSelector:@selector(applicationDidBecomeActive:)])
        [delegate applicationDidBecomeActive:UIApplication.sharedApplication];
}

- (void)sceneWillResignActive:(UIScene*)scene
{
    const id<UIApplicationDelegate> delegate = UIApplication.sharedApplication.delegate;
    if ([delegate respondsToSelector:@selector(applicationWillResignActive:)])
        [delegate applicationWillResignActive:UIApplication.sharedApplication];
}

- (void)sceneWillEnterForeground:(UIScene*)scene
{
    const id<UIApplicationDelegate> delegate = UIApplication.sharedApplication.delegate;
    if ([delegate respondsToSelector:@selector(applicationWillEnterForeground:)])
        [delegate applicationWillEnterForeground:UIApplication.sharedApplication];
}

- (void)sceneDidEnterBackground:(UIScene*)scene
{
    const id<UIApplicationDelegate> delegate = UIApplication.sharedApplication.delegate;
    if ([delegate respondsToSelector:@selector(applicationDidEnterBackground:)])
        [delegate applicationDidEnterBackground:UIApplication.sharedApplication];
}

@end

void waitForActiveScene()
{
    while (true)
    {
        @autoreleasepool
        {
            for (UIScene* scene in UIApplication.sharedApplication.connectedScenes) // NOLINT(cppcoreguidelines-init-variables)
                if (scene.activationState == UISceneActivationStateForegroundActive &&
                    [scene.delegate isKindOfClass:SFTestSceneDelegate.class])
                    return;
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.01, true);
        }
    }
}

void showTestResult(int result, const char* mode)
{
    for (UIScene* scene in UIApplication.sharedApplication.connectedScenes) // NOLINT(cppcoreguidelines-init-variables)
    {
        if (![scene.delegate isKindOfClass:SFTestSceneDelegate.class])
            continue;
        auto* const     windowScene = static_cast<UIWindowScene*>(scene);
        UIWindow* const window      = [[UIWindow alloc] initWithWindowScene:windowScene];
        window.frame                = windowScene.coordinateSpace.bounds;
        window.rootViewController   = [[UIViewController alloc] init];
        UIView* const view          = window.rootViewController.view;
        view.backgroundColor        = result == 0 ? UIColor.systemGreenColor : UIColor.systemRedColor;

        UILabel* const label   = [[UILabel alloc] initWithFrame:view.bounds];
        label.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        label.textAlignment    = NSTextAlignmentCenter;
        label.numberOfLines    = 0;
        label.textColor        = UIColor.whiteColor;
        label.font             = [UIFont boldSystemFontOfSize:28];
        NSString* const title  = [NSString stringWithFormat:@"%@\n%s", NSProcessInfo.processInfo.processName, mode];
        label.text             = result == 0 ? [title stringByAppendingString:@"\nAll tests passed"]
                                             : [title stringByAppendingFormat:@"\n%d test(s) failed", result];
        [view addSubview:label];

        static_cast<SFTestSceneDelegate*>(scene.delegate).window = window;
        [window makeKeyAndVisible];
    }
    while (true)
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1.0, false);
}
