/* Controller */

#import <Cocoa/Cocoa.h>

@interface Controller : NSObject
{
    IBOutlet id converter;
    IBOutlet id dollarField;
    IBOutlet id rateField;
    IBOutlet id totalField;
}
- (IBAction)convert:(id)sender;
@end
