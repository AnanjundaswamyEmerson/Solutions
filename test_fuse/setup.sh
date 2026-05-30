# 1. Create a blank 10MB file acting as your fake NVMe drive
dd if=/dev/zero of=mock_nvme.img bs=1M count=10

# 2. Find an open loop device slot and link it
sudo losetup -f mock_nvme.img

# 3. Check what slot it took (e.g., /dev/loop0)
losetup -a