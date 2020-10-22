



*) Do:

	PASS 1:

	For each node enumerated:
	
		If we're part of a group, and the group has the word INSTANCE: in its
		name, then skip this completely.
		
		If we're a group, and we have the word INSTANCE: in the title, record
		us as a special reference spot.
		
		If we're a group, and we have the word INSTANCE: in the title, and
		we're not a reference, then mark that this group needs to be stashed in
		the worldmesh file as a seperate little worldmesh.  This is the STASH LATER list.
		
	PASS 2:
	
	Now, go through the STASH LATER list...
	
		For each node enumerated:
		
			If we're in a group, and can trace ourselves back to the current group
			from the STASH LATER list, compile us in.
			
		
		
		
		

	
	
	
   
  
   
  
   
   

