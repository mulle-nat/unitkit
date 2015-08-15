#import "Converter.h"

@implementation Converter

-(float)convertAmount:(float)amt atRate:(float)rate
{
    return (amt * rate);
}

@end
