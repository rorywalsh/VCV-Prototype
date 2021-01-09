<CsoundSynthesizer>
<CsOptions>
-n -d 
</CsOptions>
<CsInstruments>
; Initialize the global variables. 
ksmps = 32
nchnls = 2
0dbfs = 1
;Rory Walsh 2021

gSText init "Welcome to Csound VCV-Prototype. Did you know Csound can run on all major platforms, including iOS, and Android. It can also run in all major browsers. Did you know there are plugin wrappers for VST, AU, Unity and UE4. There are also language wrappers in C/C++, Python, Lua, JS, Java, C#, etc. And it can be used direclty in Pd and Max using the csound~ and csoundapi~ objects."

//utilities opcodes to control lights
opcode switchLight, 0, kkkk
    kNumber, kR, kG, kB xin
    chnset kR, sprintfk("sl%dr", kNumber)
    chnset kG, sprintfk("sl%dg", kNumber)
    chnset kB, sprintfk("sl%db", kNumber)
endop

//utilities opcodes to control lights
opcode light, 0, kkkk
    kNumber, kR, kG, kB xin
    chnset kR, sprintfk("l%dr", kNumber)
    chnset kG, sprintfk("l%dg", kNumber)
    chnset kB, sprintfk("l%db", kNumber)
endop

instr 1
    kFreq1 randh 1000, 3, 1
    kFreq2 randh 1000, 3, 1
    kFreq3 randh 1000, 3, 1
    kFreq4 randh 1000, 3, 1
    kFreq5 randh 1000, 3, 1
    kFreq6 randh 1000, 3, 1

    a1 oscili 1, kFreq1
    a2 oscili 1, kFreq2
    a3 oscili 1, kFreq3
    a4 oscili 1, kFreq4
    a5 oscili 1, kFreq5
    a6 oscili 1, kFreq6

    chnset a1*chnget:k("k1"), "out1"
    chnset a2*chnget:k("k2"), "out2"
    chnset a3*chnget:k("k3"), "out3"
    chnset a4*chnget:k("k4"), "out4"
    chnset a5*chnget:k("k5"), "out5"
    chnset a6*chnget:k("k6"), "out6"

    kNumber = 1
    if metro(5) == 1 then
        while kNumber < 7 do
        switchLight kNumber, random:k(0, 1), random:k(0, 1), random:k(0, 1)
        light kNumber, random:k(0, 1), random:k(0, 1), random:k(0, 1)
        kNumber += 1
        od
    endif
endin

;silly ticker tape advertisement for Csound! ;) 
instr TickerTape
kStart init 0
kEnd init 20
iLength = strlen(gSText)

if metro(5) == 1 then
    event "i", "DisplayText", 0, 0, kStart, kEnd
    kEnd+=1
    kStart+=1

    if kEnd >= iLength then
        kStart = 0
        kEnd = 20
    endif
endif
endin

instr DisplayText
    SString strsub gSText, p4, p5
    prints SString
endin

</CsInstruments>
<CsScore>
i1 0 z
i"TickerTape" 0 z
</CsScore>
</CsoundSynthesizer>