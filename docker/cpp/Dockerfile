# docker pull nvidia/opengl:base-ubuntu22.04
FROM nvidia/opengl:base-ubuntu22.04

# >>>>>>>>>>>>>>>>>>>>>>>>> user settitngs >>>>>>>>>>>>>>>>>>>>>>>>>
# Update and install sudo (if not already installed in your base image)
ENV USER=docker
RUN apt-get update && apt-get install -y sudo zsh wget git curl cmake build-essential

# Create a new user 'myuser' and add it to the 'sudo' group
RUN useradd -m ${USER} && echo "${USER}:${USER}" | chpasswd && adduser ${USER} sudo

# Give '${USER}' passwordless sudo privileges
RUN echo "${USER} ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Set zsh as the default shell for the user
RUN chsh -s /bin/zsh ${USER}

# Switch to the new user
USER ${USER}:${USER}
# <<<<<<<<<<<<<<<<<<<<<<<<< user settitngs <<<<<<<<<<<<<<<<<<<<<<<<<

# >>>>>>>>>>>>>>>>>>>>>>>>> initialize >>>>>>>>>>>>>>>>>>>>>>>>>
RUN mkdir /home/${USER}/code
WORKDIR /home/${USER}/code

# eigen 
RUN sudo apt-get install -y libeigen3-dev
# x11, and xserver
RUN sudo apt-get install -y libx11-dev libxrandr-dev libxi-dev libxinerama-dev libxcursor-dev x11-apps
# opengl
RUN sudo apt-get install -y libglfw3-dev libglew-dev libglm-dev
# <<<<<<<<<<<<<<<<<<<<<<<<< initialize <<<<<<<<<<<<<<<<<<<<<<<<<

# >>>>>>>>>>>>>>>>>>>>>>>>> oh-my-zsh >>>>>>>>>>>>>>>>>>>>>>>>>
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
WORKDIR /home/${USER}/code

# set the default shell to zsh
CMD [ "zsh" ]
# <<<<<<<<<<<<<<<<<<<<<<<<< oh-my-zsh <<<<<<<<<<<<<<<<<<<<<<<<<
