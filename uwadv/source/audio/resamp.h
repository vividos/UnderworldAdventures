/****************************************************************************
*
* Name: resamp.h
*
* Synopsis:
*
*   Resamples a signal.  For more information about resampling, see dspGuru's
*   Multirate FAQ at:
*
*       http://www.dspguru.com/info/faqs/mrfaq.htm
*
* Description: See function descriptons below.
*
* by Grant R. Griffin
* Provided by Iowegian's "dspGuru" service (http://www.dspguru.com).
* Copyright 2001, Iowegian International Corporation (http://www.iowegian.com)
*
*                          The Wide Open License (WOL)
*
* Permission to use, copy, modify, distribute and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice and this license appear in all source copies. 
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
* ANY KIND. See http://www.dspguru.com/wol.htm for more information.
*
*****************************************************************************/

/*****************************************************************************
Description:

    resamp - "resamples" a signal by changing its sampling rate by a
              rational factor via FIR filtering.

    Two similar implementations are supplied, resamp0 and resamp1, which can
    be selected via the "#define resamp" below.  resamp0 is simpler (and 
    therefore easier to understand), but resamp1 is faster.

Inputs:

    interp_factor_L:
        the interpolation factor (must be >= 1)

    decim_factor_M:
        the decimation factor (must be >= 1)

    num_taps_per_phase:
        the number of taps per polyphase filter, which is the total number of
        taps in the filter divided by interp_factor_L.

    p_H:
        pointer to the array of coefficients for the resampling filter.
        (The number of total taps in p_H is interp_factor * num_taps_per_phase,
        so be sure to design your filter using a number of taps which is evenly
        divisible by interp_factor.)

    num_inp:
        the number of input samples

    p_inp:
        pointer to the input samples

Input/Outputs:

    p_current_phase:
        pointer to the current phase (must be >= 0 and <= interp_factor_L)

    p_Z:
        pointer to the delay line array (must have num_taps_per_phase
        elements)    

Outputs:

    p_out:
        pointer to the output sample array.

    p_num_out:
        pointer to the number of output samples

*****************************************************************************/

#define resamp resamp1          /* select resamp0 or resamp1 here */

/* resamp0 is simpler but slower */

void resamp0(int interp_factor_L, int decim_factor_M, int num_taps_per_phase,
             int *p_current_phase, const double *const p_H, double *const p_Z,
             int num_inp, const double *p_inp, double *p_out, int *p_num_out);

/* resamp1 is more complicated but faster */

void resamp1(int interp_factor_L, int decim_factor_M, int num_taps_per_phase,
             int *p_current_phase, const double *const p_H, double *const p_Z,
             int num_inp, const double *p_inp, double *p_out, int *p_num_out);

/*****************************************************************************
Description:

    resamp_complex - similar to resamp, above, except that it filters complex
                     (real and imaginary) inputs and outputs, using a real
                     filter.

*****************************************************************************/

void resamp_complex(int interp_factor_L, int decim_factor_M,
            int num_taps_per_phase, int *p_current_phase,
            const double *const p_H, double *const p_Z_real,
            double *const p_Z_imag, int num_inp, const double *p_inp_real,
            const double *p_inp_imag, double *p_out_real,
            double *p_out_imag, int * p_num_out);
