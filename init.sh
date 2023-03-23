OS_DIR=/workspaces/mikanos-devcontainer/mikanos_dev
ln -sfn /workspaces/mikanos-devcontainer/mikanos_dev/MikanLoaderPkg ./
# export PATH=$PATH:/usr/local/x86_64-linux-gnu/bin
# source ~/.bashrc
source ~/osbook/devenv/buildenv.sh
cd ~/edk2
source edksetup.sh
cd $OS_DIR