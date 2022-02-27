# Pins on each cable
And how they are connected to each other in this tester:

|XLR|TRS|TS|SpeakOn|
|-|-|-|-|
|2|T|T|1+|
|3|R|S|1-|
|1|S|S|2+|
|/|/|/|2-|

# Expected test results

## XLR (Stereo)

| |2|3|1|/|
|-|-|-|-|-|
|2|X|O|O|O|
|3|O|X|O|O|
|1|O|O|X|O|
|/|O|O|O|O|

## 1/4" TRS (Stereo)

| |T|R|S|/|
|-|-|-|-|-|
|T|X|O|O|O|
|R|O|X|O|O|
|S|O|O|X|O|
|/|O|O|O|O|

## 1/4" TS (Mono)

| |T|R|S|/|
|-|-|-|-|-|
|T|X|X|O|O|
|R|X|X|O|O|
|S|O|O|X|O|
|/|O|O|O|O|

## SpeakOn (NL2)

| |1+|1-|/|/|
|-|-|-|-|-|
|1+|X|O|O|O|
|1-|O|X|O|O|
|/|O|O|O|O|
|/|O|O|O|O|

## SpeakOn (NL4)

| |1+|1-|2+|2-|
|-|-|-|-|-|
|1+|X|O|O|O|
|1-|O|X|O|O|
|2+|O|O|X|O|
|2-|O|O|O|X|