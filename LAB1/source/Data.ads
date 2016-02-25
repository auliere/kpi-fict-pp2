generic 
	Size: Integer;
	Random_Max: Integer;
package Data is
	subtype Range_T is Integer range 1 .. Size;
	type Vector is array(Range_T) of Float;
	type Matrix is array(Range_T) of Vector;
	
	procedure Input(A: out Vector);
	procedure Input(A: out Matrix);
	
	procedure Output(A: in Vector);
	procedure Output(A: in Matrix);
	procedure Output(A: in Float);
	
	procedure Get_Random(A: out Float);
	
private


end Data;