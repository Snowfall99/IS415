all: firmiana listen

firmiana:
	g++ -std=c++11 main.cc -o firmiana
	sudo cp ./firmiana /usr/bin/firmiana

listen:
	g++ -std=c++11 daemon.cc -o listen

clean:
	rm firmiana listen
