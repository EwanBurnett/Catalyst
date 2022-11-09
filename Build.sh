echo "Building Catalyst" 

#Run CMake from /Build/
if [ ! -d "Build" ]; then
	echo "Creating build directory..."
	mkdir Build
fi

cd Build && time cmake ..
