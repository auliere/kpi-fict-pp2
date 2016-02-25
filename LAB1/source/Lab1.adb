---------------------------------------------------------
------------------PARALLEL PROGRAMMING-------------------
------------------------LAB #1---------------------------
----       ---------ADA. SEMAPHORES----------       -----
----              MA = MB*MC + MO*ME*a              -----
-----------------CREATED ON 24.02.2016-------------------
----------------BY OLEG PEDORENKO, IP-31-----------------
---------------------------------------------------------

with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Ada.Synchronous_Task_Control; use Ada.Synchronous_Task_Control;
with Ada.Float_Text_IO; 	use Ada.Float_Text_IO;
with Ada.Real_Time; 	use Ada.Real_Time;
with Ada.Calendar; use Ada.Calendar;
with Ada.Command_Line; use Ada.Command_Line;
with Data;

procedure Lab1 is

	N: Integer := 8;

begin
	if(Argument_Count > 0) then 
		N := Integer'Value(Argument(1));
	end if;
	
	declare
		-- Константы
		P: Integer := 2;
		H: Integer := N/P;
		S: Suspension_Object;
		t0: Ada.Real_Time.Time;
		t1: Ada.Real_Time.Time;
		
		--Типы
		package Data_S is new Data(N, 100); use Data_S;
		
		--Переменные
		MA, MB, MC, MO, ME: Matrix;
		a: Float;
		
		--Semaphores
		Sem1, Sem2, Skd: Suspension_Object;

		procedure Start_Tasks is		
			
			task T1 is
				pragma Storage_Size(1000000000);
				pragma CPU(0);
			end T1;
			task body T1 is
				MBx, MOx: Matrix;
				ax: Float;
				Sum1: Float;
				Sum2: Float;
			begin
				Put_Line("Task 1 started");
				--Enter MB, MC, MO, ME, a			V
				Put_Line("MA =   "); Input(MA);
				Put_Line("MB =   "); Input(MB); 
				Put_Line("MC =   "); Input(MC); 
				Put_Line("MO =   "); Input(MO); 
				Put_Line("ME =   "); Input(ME);	
				Get_Random(a);
				Put("A =    "); Put(a); Put_Line("");
				
				t0 := Clock;
				--Signal S2,1---------------------------
				Set_True(Sem1);
				-- --P(Skd)
				-- Suspend_Until_True(Skd);
				-- --Copy MB, MO, a -- Critical section
				-- MBx := MB;
				-- MOx := MO;
				-- ax := a;
				-- --V(Skd)
				-- Set_True(Skd);
				--MAh = MBX*MCh + MOX*MEh*ax			
				for I in 1..N loop			
					for J in 1..H loop
						Sum1 := 0.0;
						Sum2 := 0.0;					
						for K in 1..N loop			
							Sum1 := Sum1 + (MB(I)(K) * MC(K)(J)); -- MBx * MCh
							Sum2 := Sum2 + (MO(I)(K) * ME(K)(J)); -- MOx * MEh				
						end loop;
						MA(I)(J) := Sum1 + Sum2*a;
					end loop;				
				end loop;
				--Wait W2,1---------------------------
				Suspend_Until_True(Sem2);
				--Output of MA
				Put_Line("MA =   "); Output(MA);			
				Put_Line("Task 1 Finished");
				Set_True(S);
			end T1;
			
			task T2 is
				pragma Storage_Size(1000000000);
				pragma CPU(1);
			end T2;
			task body T2 is
				MBx, MOx: Matrix;
				ax: Float;
				Sum1: Float;
				Sum2: Float;			
			begin
				Put_Line("Task 2 started");
				--Wait W1,1---------------------------
				Suspend_Until_True(Sem1);
				-- --P(Skd)
				-- Suspend_Until_True(Skd);
				-- --Copy MB, MO, a -- Critical section
				-- MBx := MB;
				-- MOx := MO;
				-- ax := a;
				-- --V(Skd)
				-- Set_True(Skd);
				--MAh = MBX*MCh + MOX*MEh*ax			
				for I in 1..N loop
					for J in (H+1)..N loop
						Sum1 := 0.0;
						Sum2 := 0.0;
						for K in 1..N loop
							Sum1 := Sum1 + (MB(I)(K) * MC(K)(J)); --MBx * MCh
							Sum2 := Sum2 + (MO(I)(K) * ME(K)(J)); -- MOx * MEh
						end loop;
						MA(I)(J) := Sum1 + Sum2*a;
					end loop;				
				end loop;
				--Signal S1,1---------------------------			
				Set_True(Sem2);
				
				Put_Line("Task 2 finished");
			end T2; 
			
		begin
			null;		
		end Start_Tasks;
		
	begin
		Set_True(Skd);
		Start_Tasks;
		Suspend_Until_True(S);
		t1 := Clock;
		Put("N =   "); Put(N);
		Put("      Work time: ");
		Put(Duration'Image(Ada.Real_Time.To_Duration(t1 - t0))); Put_Line("s. ");
	end;
end Lab1;