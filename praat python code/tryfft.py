# -*- coding: utf-8 -*-
"""
Created on Sat Dec 03 15:12:57 2016

@author: Qisen
"""
import numpy, math
import matplotlib.pyplot as plt

t = range(1000)
f0 = 50
fs = 1024

sig = [math.sin(2*numpy.pi*x*f0/fs) for x in t]
n = len(sig)
K = numpy.arange(n)
#print K
freq = K*1.0/(2*n)*float(fs)
#print freq
freq = freq[range(n/2)] 
            
amp0 = numpy.fft.fft(numpy.append(sig, numpy.zeros(n)))
amp = amp0[range(n/2)]
ampNorm = abs(amp0)
ampConj = numpy.conj(amp0)
autoCorr = numpy.fft.ifft(amp0*ampConj)
print len(autoCorr)
auto = numpy.append(autoCorr[len(autoCorr)/2+1:], autoCorr[:len(autoCorr)/2])
#auto
acRef = numpy.correlate(sig, sig, mode = 'full')
#print ('auto',auto,len(auto))
print ('acRef', acRef, len(acRef))
reSig = numpy.fft.ifft(amp0)          
#print reSig
          
fig, ax = plt.subplots(2, 1)
#==============================================================================
ax[0].plot(t,sig)
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Amplitude')
ax[1].plot(freq,abs(amp),'r') # plotting the spectrum
ax[1].set_xlabel('Freq (Hz)')
ax[1].set_ylabel('|Y(freq)|') 
# ax[2].plot(t, reSig.real, 'b-', t, reSig.imag, 'r--') # plotting the spectrum
# ax[2].set_xlabel('Time')
# ax[2].set_ylabel('Amplitude')
#==============================================================================
#ax[0].plot(numpy.arange(-len(t)+1,len(t)),acRef) # plotting the spectrum
#ax[0].set_xlabel('Lag')
#ax[0].set_ylabel('Auto')     
#ax[1].plot(numpy.arange(-len(t)+1,len(t)), auto) # plotting the spectrum
#ax[1].set_xlabel('Lag')
#ax[1].set_ylabel('Auto')      
#print ([x for x in freq.real])
#amp = [numpy.sqrt(numpy.power(x,2)+numpy.power(y,2)) for x in freq.real for y in freq.imag]
#print (amp)
#plt.plot (amp)
#plt.show()