
#ifndef _filter_h_included_
#define _filter_h_included_

extern "C" void computeLowpassFilter
(
    int M, double fc,
    double * w // must be M+1 values in array
);


#endif

