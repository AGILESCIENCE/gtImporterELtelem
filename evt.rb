Dir["baric/*/*.EVT"].sort.each do | file |
	cmd = "./gtImporterELtelem 0 " + file.to_s
	puts cmd
	system cmd
end
