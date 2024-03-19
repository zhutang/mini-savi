# mini-savi
mininet combined with savi for satellite network simulation
# install
for ubuntu 18.04 / 22.04
1. apt-get install geomview
2. apt-get install mininet
3. apt-get install tk-dev
3. git clone https://github.com/zhutang/mini-savi.git
4. cd mini-savi; make ARCH=linux

# run
1. cd mini-savi/mini-savi; python3 router-host.py
2. in another terminal, input: cd mini-savi; geomview -run ./savi
3. select constellation, for example iridium, and click run
