# 10520CS423300-Network-Programming-Practic4-10Apr2017
		
In-class	practice	#4:	Before	21:30			
			
To	understand	setsockopt().			
Please	modify	the	echo	server/client	programs	to	achieve	the	following	check	points:	 					
		
Check	point	1:	(30%)			
1. Use	“TCP	keep	alive”	to	establish	connection			
2. Shutdown	echo	server/client	and	immediately	re-execute	echo			
		
    server/client	→ bind	error			
	
Check	point	2:	(30%)	
		
  Shutdown	echo	server/client	and	immediately	re-execute	echo	server/client	to			
		
  re-establish	a	successful	connection	by	“TCP	port	reuse	socket	option”.			
	
Check	point	3:	(40%)			
		
  Shutdown	echo	server/client	and	immediately	re-execute	echo	server/client	to			
		
  re-establish	a	successful	connection	by	linger.			
	
  		
Hint:	TAs	will	use	“watch	-d	-n	1	‘netstat	--timer	|	grep	[port	number]’	“to	monitor			
		
the	connection	states.		
