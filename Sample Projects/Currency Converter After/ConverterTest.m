//
//  ConverterTest.m
//  Currency Converter
//
//  Created by James Davidson on Tue Mar 09 2004.
//  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
//

#import "ConverterTest.h"
#import "Converter.h"

@implementation ConverterTest

- (void) testConverter
{
    float initialAmt = 27.30;
    float rate = 3.33;
    Converter *converter = [[Converter alloc] init];    
    float convertedAmt = [converter convertAmount:initialAmt atRate:rate];    
    UKFloatsEqual(90.90, convertedAmt, 0.01);
    [converter release];
}

@end
