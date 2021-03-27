VERSION=3.33

install:
	sudo -S sh dependencies.sh
	if [ -d "./ns-allinone-$(VERSION)" ]; then \
		echo "Dir exists, skip downloading .."; \
	else \
		wget http://www.nsnam.org/release/ns-allinone-$(VERSION).tar.bz2; \
		tar -xf ./ns-allinone-$(VERSION).tar.bz2; \
		rm ./ns-allinone-$(VERSION).tar.bz2; \
	fi
	cd ns-allinone-$(VERSION) && ./build.py --enable-examples --enable-tests
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf configure
run:
	yes | cp -rf src/* ns-allinone-$(VERSION)/ns-$(VERSION)/scratch/
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf --run Simulation
vis:
	yes | cp -rf src/* ns-allinone-$(VERSION)/ns-$(VERSION)/scratch/
	cd ns-allinone-$(VERSION)/ns-$(VERSION)/ && ./waf --run Simulation --vis