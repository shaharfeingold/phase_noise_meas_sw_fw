# phase_noise_meas_sw_fw
this git including all the src files need to run the software side and firmware side of our phase noise measurment device. you should have also a red pitaya FPGA with a bit file generate from the vivado project [cited here](https://github.com/shaharfeingold/phase_noise_measurment_logic)

## How to use it ?

### Installing FW side
1. first you will need to download the fw directory to your local computer
2. connect to red pitya termianl by using your preferefable methods either by ssh or using 3<sup>rd</sup> party software like putty.
3. create a new directory under any prefable location and following below instructions:
```
mkdir phase_noise_mease
cd phase_noise_mease
mkdir fw
cd fw
mkdir workdir
```
4. copy all the src file downloaded from the git and place them under fw directory you have just created.
5. go to `workdir` dir you have just created and do the following:
```
rm ../src/*.gch
gcc ../src/client_connection.c ../src/client_connection.h ../src/defines.h ../src/event_watcher.c ../src/event_watcher.h ../src/fw_data_struct.c ../src/fw_data_struct.h ../src/logic_config.c ../src/logic_config.h ../src/main.h ../src/main_flow.c ../src/read_write.c ../src/read_write.h ../src/utils_function.c ../src/utils_function.h  -lpthread 2> comp.log
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