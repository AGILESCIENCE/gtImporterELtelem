Dir["/Volumes/TOSHIBAEXT/agile/DATA_ASDCSTDk/LOG/*.gz"].sort.each do | file |
	cmd = "./gtImporterELtelem 0 " + file.to_s
	puts cmd
	system cmd
end
