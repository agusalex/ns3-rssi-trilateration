VERSION=3.33
# Dependencies work for Ubuntu 20.04
install:
	sudo -S sudo apt-get update && sudo apt-get install -y \
            build-essential autoconf automake libxmu-dev \
             python3-pygraphviz cvs mercurial bzr git cmake \
              python3-matplotlib python-tk python3-dev qt5-qmake \
               gnuplot-x11 p7zip-full qt5-default gir1.2-goocanvas-2.0

	if [ -d "./ns-allinone-$(VERSION)" ]; then \
		echo "Dir exists, skip downloading .."; \
	else \
		wget http://www.nsnam.org/release/ns-allinone-$(VERSION).tar.bz2; \
		tar -xf ./ns-allinone-$(VERSION).tar.bz2; \
		rm ./ns-allinone-$(VERSION).tar.bz2; \
	fi
	#cd ns-allinone-$(VERSION) && ./build.py --enable-examples --enable-tests
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf configure
run:
	yes | cp -rf src/* ns-allinone-$(VERSION)/ns-$(VERSION)/scratch/
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf --run Simulation
vis:
	yes | cp -rf src/* ns-allinone-$(VERSION)/ns-$(VERSION)/scratch/
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf --run Simulation --vis