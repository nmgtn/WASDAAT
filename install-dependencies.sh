# Prep the project directory if it doesn't already have the right structure
mkdir -p lib
mkdir -p include

# Install portsf (places archive and header in lib and include directories)
cd portsf
make install
