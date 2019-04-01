pkg:
	mkdir -p build/DEBIAN
	mkdir -p build/ontowasm
	cp control build/DEBIAN
	cp -rf ../install/* build/ontowasm/
	dpkg-deb -b build ontowasm-ubuntu18.04-amd64.deb

clean:
	rm -rf build ontowasm-ubuntu18.04-amd64.deb
