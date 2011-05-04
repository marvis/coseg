all:
	cd create_view && make
	cd coseg && make
	cd ../AT3D/ && make
install:
	cp coseg/coseg bin/
	cp coseg/coseg0 bin/
	cp coseg/coseg1 bin/
	cp create_view/create_tree bin/
	cp create_view/view_tree bin/
	cp create_view/tree_graph bin/
	cp ../AT3D/AT3D bin/new_AT3D
clean:
	cd create_view && make clean
	cd coseg && make clean
