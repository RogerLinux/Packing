#Mine quantity: 32
#Max radius: 200
#min radius: 100
#length of the area: 1600
#width of the area: 1200

#Arrange the circles
#The first type input that includes randomly assigned radii
echo
echo The first try ...
echo ==========================
./Packing 32 200 100 1600 1200 plot1.py

#The second type input that includes designated radii
echo
echo The second try ...
echo ==========================
./Packing radii 1600 1200 plot2.py

echo finish, use python plot*.py to check the visualized results
