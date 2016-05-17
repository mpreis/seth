# Seth
Analysis of security evaluation methods for chaos-based encryptions.

## Abstract
Over the past years a lot of different chaos-based image encryption algorithms has been proposed. In various experiments and analysis it is shown by the authors, that their encryption algorithm justifies a high security performance. This is for example done by analyzing the statistical behaviors of an encrypted image, the key sensitivity or the key space of the algorithm. There are common ways and methods to do this. Our assertion is, that just passing these tests does not guarantee a strong encryption algorithm or a high security performance. We show this by implementing obviously insecure algorithms and compare the results with well known chaos-based encryption schemes.

## Building [![Build Status](https://drone.io/github.com/mpreis/seth/status.png)](https://drone.io/github.com/mpreis/seth/latest)

This is as easy as

    cd /path/to/seth/
    make

The executable can be found in

    seth/build/

## Usage

Call Seth like this

    ./build/seth <flags>

### Flags

List of available flags
  * `-h` help
  * `-l` produce less output files (only final result)
  * `-d <directory>` directory of source images
  * `-t {<evaluation method>}` list of evaluation methods  to executed (min. 1)
  * `-e {<encryption>}` list of encryptions to executed (min. 1)
  * `-k <number of keys>` number of random generated keys applied on each image (default: 1)

#### Evaluation methods
  * `all` runs all tests
  * `npcr` NPCR
  * `uaci` UACI
  * `grsh` gray scale histogram
  * `grsv` gray scale histogram value
  * `kese` key sensitivity
  * `cprtv` correlation property test vertical
  * `cprth` correlation property test horizontal
  * `cprtd` correlation property test diagonal
  * `seqts` sequence test single bit
  * `seqtd` sequence test double bit
  * `entri` entropy of image
  * `nist` NIST test suite

#### Encryptions
  * `all` runs all encryptions
  * `origin` no encryption (use original image)
  * `arnold` Arnold cat's map
  * `baker` Baker's map
  * `baker-sub` baker's map in substitution mode
  * `xor-otp-rc4-msb` XOR OTP RC4 in MSB mode
  * `xor-otp-rc4-pix` XOR OTP RC4 in pixel mode
  * `xor-otp-cstd-msb` XOR OTP CSTD in MSB mode
  * `xor-otp-cstd-pix` XOR OTP CSTD in pixel mode
  * `xor-key-msb-8` XOR short-key in MSB mode 8 bit key
  * `xor-key-pix-8` XOR short-key in pixel mode 8 bit key
  * `xor-key-msb-32` XOR short-key in MSB mode 32 bit key
  * `xor-key-pix-32` XOR short-key in pixel mode 32 bit key
  * `xor-key-msb-256` XOR short-key in MSB mode 256 bit key
  * `xor-key-pix-256` XOR short-key in pixel mode  256 bit key
  * `xor-followers-8` XOR followers 8 bit key
  * `xor-followers-32` XOR followers 32 bit key
  * `xor-followers-256` XOR followers 256 bit key
  * `2d-log-map-256` 2 dimensional logistic chaotic map 256 bit key

## License

Copyright (c) 2016, the Seth Project Authors. All rights reserved. 
Please see the AUTHORS file for details.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


