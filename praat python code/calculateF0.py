# -*- coding: utf-8 -*-
"""
Created on Sat Dec 03 13:22:04 2016

@author: Qisen
"""
import numpy, math, copy
import matplotlib.pyplot as plt
from operator import add

def interpolateLinear(
        y1, #
        y2, #
        x # weighting [0..1]. 0 would be 100 % y1, 1 would be 100 % y2
):
    return y1 * (1.0 - x) + y2 * x


def interpolateParabolic(
        alpha, 
        beta, 
        gamma, 
        x # relative position of read offset [-1..1]
):
    if (x == 0): return beta
     
    #we want all numbers above zero ...
    offset = alpha;
    if (beta < offset): offset = beta
    if (gamma < offset): offset = gamma
    offset = math.fabs(offset) + 1
     
    alpha += offset;
    beta += offset;
    gamma += offset;
     
    a = b = c = 0;
    a = (alpha - 2.0 * beta + gamma) / 2.0
    if (a == 0):
        if (x > 1):
            return interpolateLinear(beta, gamma, x) - offset
        else:
            return interpolateLinear(alpha, beta, x + 1) - offset
    else:
        c = (alpha - gamma) / (4.0 * alpha)
        b = beta - a * c * c
        return (a * (x - c) * (x - c) + b) - offset


def findArrayMaximum(
         data, 
         offsetLeft = 0, 
         offsetRight = -1, # if -1, the array size will be used
         doInterpolate = True, # increase accuracy by performing a 
                               # parabolic interpolation
 ):
    objType = type(data).__name__.strip()
    if objType <> "ndarray":
        raise Exception('data argument is no instance of numpy.array')
    size = len(data)
    if (size < 1):
        raise Exception('data array is empty')
    xOfMax = -1
    valMax = min(data)
    if offsetRight == -1:
       offsetRight = size
    for i in range(offsetLeft + 1, offsetRight - 1):
        if data[i] >= data[i-1] and data[i] >= data[i + 1]:
            if data[i] > valMax:
                valMax = data[i]
                xOfMax = i
    if doInterpolate:
        if xOfMax > 0 and xOfMax < size - 1:
            # use parabolic interpolation to increase accuracty of result
            alpha = data[xOfMax - 1]
            beta = data[xOfMax]
            gamma = data[xOfMax + 1]
            xTmp = (alpha - gamma) / (alpha - beta * 2 + gamma) / 2.0
            xOfMax = xTmp + xOfMax
            valMax = interpolateParabolic(alpha, beta, gamma, xTmp)
    return [xOfMax, valMax]

def windowFunc(size, winType):
#    data = numpy.zeros(size)
#     for i in range(size):
#         xrel = float(i) / float(size)
#         if type == LOOKUP_TABLE_NONE:
#             tmp = 1
#         elif type == LOOKUP_TABLE_SINE:
#             tmp = math.sin (xrel * math.pi * 2)
#         elif type == LOOKUP_TABLE_COSINE:
#             tmp = math.cos (xrel * math.pi * 2)
#         elif type == LOOKUP_TABLE_HAMMING:
#             tmp = 0.54 - 0.46 * math.cos(2 * math.pi * xrel)
#         elif type == LOOKUP_TABLE_HANN:
#             tmp = 0.5 - 0.5 * math.cos(2 * math.pi * xrel)
#         else:
#             raise Exception('type ' + str(type) + ' not recognized')
#         data[i] = tmp
    if winType == 'Hann':
        window = numpy.hanning(size)
    return window

def calculateF0once( 
    data, 
    fs, 
    Fmin = 50,
    Fmax = 3000,
    voicingThreshold = 0.3,
    applyWindow = False
    ):
    dataTmp = copy.deepcopy(data)
   
    # apply window
    if applyWindow:
        fftWindow = windowFunc(len(dataTmp), 'Hann')
        dataTmp *= fftWindow
    
    # autocorrelation
    result = numpy.correlate(dataTmp, dataTmp, mode = 'full')
    r = result[result.size/2:] / float(len(data))
    
    #print (r)
    # find peak in AC 
    try:
        xOfMax, valMax = findArrayMaximum(r, fs / Fmax, \
            fs / Fmin)
        valMax /= max(r)
        freq = fs / xOfMax
        return freq
    except Exception as e:
        #print e
        return 0


def calculateF0( 
    data, 
    fs, 
    Fmin = 50,
    Fmax = 3000,
    numPeriods = 5.0,
    progressPeriods = 1,
    voicingThreshold = 0.3,
    applyWindow = True
    ):
    numFrames = len(data)
    readSize = int(numPeriods * float(fs) / Fmin)
    offset = 0
    #numCycles = 0
    arrF0 = { 't':[], 'freq':[] }
    #F0progressPeriods = progressPeriods
    while (offset < numFrames):
        dataTmp = numpy.zeros(readSize)
        #print (range(readSize))
        for i in range(readSize):
            idx = int(i + offset - (readSize / 2))
            #print ('idx', idx)
            if idx >= 0 and idx < numFrames:
                dataTmp[i] = data[idx]
        
        freq = calculateF0once(dataTmp, fs, Fmin = Fmin, Fmax = Fmax, 
            voicingThreshold = voicingThreshold, applyWindow = applyWindow)
        
        periodSize = 0
        if freq > 0:
            periodSize = fs / freq
        t = (offset + (periodSize / 2.0))/fs
        if freq >= Fmin and freq <= Fmax:
            arrF0['t'].append(t)
            arrF0['freq'].append(freq)
        else:
            # set F0 to zero if out of bounds
            arrF0['t'].append(t)
            arrF0['freq'].append(0)
            
        if periodSize > 10:
            offset += periodSize * progressPeriods
        else:
            offset += 10
    arrF0['t'] = numpy.array(arrF0['t'])
    arrF0['freq'] = numpy.array(arrF0['freq'])
    return arrF0

def genSine(f0, fs, dur):
    t = numpy.arange(dur)
    sinusoid = numpy.sin(2*numpy.pi*t*(f0/fs))
    #sinusoid = normalise(sinusoid)
    return sinusoid
    
def main():
    #sig = genSine(1000., 44100., 1000)
    t = range(1024)
    f0 = 1000
    fs = 48000
    sig1 = [math.sin(2*numpy.pi*x*f0/fs)* 1 for x in t] 
    sig2 = [math.sin(2*numpy.pi*x*300/fs)* 1 for x in t] 
    sig3 = [math.sin(2*numpy.pi*x*2*f0/fs)* 1 for x in t] 
    sig4 = [math.sin(2*numpy.pi*x*2*300/fs)* 1 for x in t] 
    sig5 = [math.sin(2*numpy.pi*x*2500/fs)* 1 for x in t] 
    sig6 = [math.sin(2*numpy.pi*x*2*2500*f0/fs)* 1 for x in t] 
    sig = [sum(x) for x in zip(sig1, sig2, sig3, sig4,sig5,sig6)]
    #print sig
    freq = numpy.fft.fft(sig)
    n = len(t)
    freq = freq[range(n/7)]
    K = numpy.arange(n)
    mm = K*1.0/(n)*float(fs)
    mm = mm[range(n/7)] 
    #print freq
    F0 = calculateF0(sig, fs)
    print ("Fundamental in each time frame:")
    i = 0
    for F0 in F0['freq']:
        i = i + 1
        print ('time_frame_' + str(i) + ': ' + str(F0) + ' Hz')
    
    matplotlib.rc('xtick', labelsize=10) 
    matplotlib.rc('ytick', labelsize=10)
    fig, ax = plt.subplots(2, 1)
    ax[0].plot(sig)
    ax[0].set_xlabel('Time')
    ax[0].set_ylabel('Amplitude')
    ax[0].set_title('Input signal & FFT spectrum')
    ax[1].plot(mm,abs(freq),'r')
    ax[1].set_xlabel('Frequency(Hz)')
    ax[1].set_ylabel('Amplitude')
    plt.show()
if __name__ == "__main__":
    main()