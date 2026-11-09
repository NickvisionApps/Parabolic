#import <Cocoa/Cocoa.h>

@interface SettingsPostprocessingArgumentEditorViewController : NSViewController <NSTextFieldDelegate>
- (instancetype)initWithName:(NSString*)name
                        step:(NSInteger)step
                  executable:(NSInteger)executable
                        args:(NSString*)args
               existingNames:(NSArray<NSString*>*)existingNames
                  completion:(void (^)(NSString* name, NSInteger step, NSInteger executable, NSString* args))completion;
@end
