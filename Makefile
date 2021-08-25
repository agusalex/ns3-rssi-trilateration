VERSION=3.33
# Dependencies work for Ubuntu 20.04
install:
#	sudo -S sudo apt-get update && sudo apt-get install -y \
            build-essential autoconf automake libxmu-dev \
             python3-pygraphviz cvs mercurial bzr git cmake \
              python3-matplotlib python-tk python3-dev qt5-qmake \
               gnuplot-x11 p7zip-full qt5-default gir1.2-goocanvas-2.0 virtualenv git\

	if [ -d "./ns-allinone-$(VERSION)" ]; then \
		echo "Dir exists, skip downloading .."; \
	else \
		git clone https://github.com/agusalex/rssi-filter-profiling \
		git clone https://github.com/agusalex/easy-trilateration \
		virtualenv env --python=python3 \
		env/bin/pip install -r rssi-filter-profiling/requirements.txt; \
		env/bin/pip install -r easy-trilateration/requirements.txt; \
		wget http://www.nsnam.org/release/ns-allinone-$(VERSION).tar.bz2; \
		tar -xf ./ns-allinone-$(VERSION).tar.bz2; \
		rm ./ns-allinone-$(VERSION).tar.bz2; \
	fi
	#cd ns-allinone-$(VERSION) && ./build.py --enable-examples --enable-tests
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf configure
copy: 
	yes | cp -rf src/* ns-allinone-$(VERSION)/ns-$(VERSION)/scratch/
run:
	make copy
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf -v --run 2ParticleFiltering
vis:
	make copy
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf --run 2ParticleFiltering --vis
graph-1d:
	make copy
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf --run 1DDistanceProfiling
	env/bin/python rssi-filter-profiling/main.py --file ns-allinone-$(VERSION)/ns-$(VERSION)/capture_1.csv \

graph-2d:
	make copy
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf --run 2ParticleFiltering
	env/bin/python easy-trilateration/main.py --file ns-allinone-$(VERSION)/ns-$(VERSION)/capture_combined.csv \
