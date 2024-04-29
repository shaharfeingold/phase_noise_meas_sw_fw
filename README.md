# phase_noise_meas_sw_fw
this git including all the src files need to run the software side and firmware side of our phase noise measurment device. you should have also a red pitaya FPGA with a bit file generate from the vivado project [cited here](https://github.com/shaharfeingold/phase_noise_measurment_logic)

## How to use it ?

### Installing and Running FW side
1. first you will need to download the fw directory to your local computer
2. connect to red pitya termianl by using your preferefable methods either by ssh or using 3<sup>rd</sup> party software like putty.
3. create a new directory under any prefable location and following below instructions:
```
mkdir phase_noise_mease
```
```
cd phase_noise_mease
```
```
mkdir fw
```
```
cd fw
```
```
mkdir workdir
```
4. copy all the src file downloaded from the git and place them under fw directory you have just created.
5. go to `workdir` dir you have just created and do the following:
```
rm ../src/*.gch
```
```
gcc ../src/client_connection.c ../src/client_connection.h ../src/defines.h ../src/event_watcher.c ../src/event_watcher.h ../src/fw_data_struct.c ../src/fw_data_struct.h ../src/logic_config.c ../src/logic_config.h ../src/main.h ../src/main_flow.c ../src/read_write.c ../src/read_write.h ../src/utils_function.c ../src/utils_function.h  -lpthread 2> comp.log
```
```
less comp.log
```
in case of good compilation you should not see any warning or errors
> [!CAUTION]
> if you encounter any warning or error please contact us for support 
> anything which is preformat with a bad compiled software could lead to unexpceted behavior and poteinally damaging the fpga
6. burn the bit file to fpga using red pitaya instruction
7. run the sever by using the follwing command under the workdir
```
./a.out 2 > output.log
```
> [!NOTE]
> every run of the server a log will apper under the workdir for further debuging in case of an error.

### Installing and Running the SW side
> [!WARNING]
> the instruction below are for running the SW on Linux/Mac based machine.

1. download from the git `src` dir.
2. on you host computer (should have able the present graphic) got to a preferable location and do the following:
```
mkdir phase_noise_mease
```
```
cd phase_noise_mease
```
```
mkdir sw
```
```
cd sw
```
```
mkdir workdir
```
3. copy all the src file (all *.py files) downloaded from the git and place them under fw directory you have just created.
4. insure the server is running by doing [the instuctions the section above](./README.md#installing-and-running-fw-side)
5. run the sw by typing the following command under `workdir`
```
../src/main.py -m 1@1.com -r <number_of_desire_repetion>
```
\replace <number_of_desire_repetion> with yours number\
6. follow on screen instrucions
7. in case any pop window appers please igonre and close it.
8. copy matlab file `data_anylsis.m` to the `workdir` and open it.
9. change `num_of_samples` on the second line the \<number_of_desire_repetion>\
10. change `f` on the 32 line to the frequncy you testing (working frequncy)
11. run matlab file 
12. you shall get 2 graphes: one represting the measuremnt out of fpga and second represting a measerment by using the sampled signal and same methods only implement in the matlab script.

> [!NOTE]
> in case you want to run in again please make sure to delete all the txt file under you workir. 

> [!NOTE]
> you can do another measurment without deleteing the previous one by copy all the workdir content to new directory.