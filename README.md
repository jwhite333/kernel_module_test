# kernel_module_test

## 1. Building:
```bash
sudo make
cd userspace
make
```

## 2. Check if module is loaded already
```bash
lsmod | grep test_module
```
- If the module is found, skip step 3

## 3\. Load Module
```bash
cd ..
sudo insmod test_module.ko
dmesg
```

Check output of dmesg, should look like:
```
[21569.277588] test_device: major number is 241
[21569.277591]  use "mknod /dev/test_device c 241 0" for device file
```

Use the command given from dmesg to create the device file, then run
```bash
sudo chmod +777 /dev/<device-file>
```

## 4. Running the example client
```bash
cd userspace
./test_app
```

## 5. Removing the module
```bash
sudo rmmod test_module
```