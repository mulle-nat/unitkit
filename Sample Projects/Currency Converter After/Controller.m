#import "Controller.h"
#import "Converter.h"							// a

@implementation Controller

- (IBAction)convert:(id)sender
{
    float rate = [rateField floatValue];				// b
    float amt = [dollarField floatValue];				// c
    float total = [converter convertAmount:amt atRate:rate];		// d
    [totalField setFloatValue:total];					// e
}

@end
