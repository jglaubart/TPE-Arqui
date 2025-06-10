docker start tpe
docker exec -it tpe make clean -C /root/Toolchain
docker exec -it tpe make clean -C /root/
docker exec -it tpe make -C /root/Toolchain
docker exec -it tpe make -C /root/
sudo chown $(whoami) Image/x64BareBonesImage.qcow2