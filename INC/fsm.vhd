-- fsm.vhd: Finite State Machine
-- Author(s): Marek Kukuèka, 1.BIT 2016/2017
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1, PRINT_FALSE, PRINT_OK , FINISH, F_TEST2, F_TEST3,F_TEST4, F_TEST5, F_TEST6, F_TEST7, F_TEST8, F_TEST9, F_TEST10, 
							S_TEST2, S_TEST3, S_TEST4, S_TEST5, S_TEST6, S_TEST7, S_TEST8, S_TEST9, S_TEST10, ERROR,F_TEST11, S_TEST11);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;
----------------------------------------------------------
--2479316125  7437948376
-- F represents FIRST
-- S represents SECOND
-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      next_state <= TEST1;
      if(KEY(2) = '1') then
			next_state <= F_TEST2;	
		elsif(KEY(7) = '1')then
			next_state <= S_TEST2;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
   
	when F_TEST2 =>
		next_state <= F_TEST2;
		if (KEY(4) = '1') then
			next_state <= F_TEST3;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
	
	when F_TEST3 => 
		next_state <= F_TEST3;
		if(KEY(7) = '1') then
			next_state <= F_TEST4;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when F_TEST4 =>
		next_state <= F_TEST4;
		if(KEY(9) = '1')then
			next_state <= F_TEST5;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when F_TEST5 =>
		next_state <= F_TEST5;
		if(KEY(3) = '1')then
			next_state <= F_TEST6;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when F_TEST6 =>
		next_state <= F_TEST6;
		if(KEY(1) = '1')then
			next_state <= F_TEST7;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when F_TEST7 =>
		next_state <= F_TEST7;
		if(KEY(6) = '1')then
			next_state <= F_TEST8;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
	
	when F_TEST8 =>
		next_state <= F_TEST8;
		if(KEY(1) = '1')then
			next_state <= F_TEST9;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;

	when F_TEST9 =>
		next_state <= F_TEST9;
		if(KEY(2) = '1') then
			next_state <= F_TEST10;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
	
	when F_TEST10 =>
		next_state <= F_TEST10;
		if(KEY(5) = '1') then
			next_state <= F_TEST11;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when F_TEST11 =>
		next_state <= F_TEST11;
		if (KEY(15) = '1') then
			next_state <= PRINT_OK;
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
		end if;
	
	when S_TEST2 =>
		next_state <= S_TEST2;
		if (KEY(4) = '1') then
			next_state <= S_TEST3;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
	
	when S_TEST3 => 
		next_state <= S_TEST3;
		if(KEY(3) = '1') then
			next_state <= S_TEST4;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when S_TEST4 =>
		next_state <= S_TEST4;
		if(KEY(7) = '1')then
			next_state <= S_TEST5;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when S_TEST5 =>
		next_state <= S_TEST5;
		if(KEY(9) = '1')then
			next_state <= S_TEST6;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when S_TEST6 =>
		next_state <= S_TEST6;
		if(KEY(4) = '1')then
			next_state <= S_TEST7;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when S_TEST7 =>
		next_state <= S_TEST7;
		if(KEY(8) = '1')then
			next_state <= S_TEST8;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
	
	when S_TEST8 =>
		next_state <= S_TEST8;
		if(KEY(3) = '1')then
			next_state <= S_TEST9;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;

	when S_TEST9 =>
		next_state <= S_TEST9;
		if(KEY(7) = '1') then
			next_state <= S_TEST10;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
	
	when S_TEST10 =>
		next_state <= S_TEST10;
		if(KEY(6) = '1') then
			next_state <= S_TEST11;
		elsif (KEY(15) = '1') then
         next_state <= PRINT_FALSE; 
		elsif (KEY(15 downto 0) /= "0000000000000000") then 
			next_state <= ERROR;
      end if;
		
	when S_TEST11 =>
		next_state <= S_TEST11;
		if (KEY(15) = '1') then
			next_state <= PRINT_OK;
	elsif (KEY(15 downto 0) /= "0000000000000000") then 
		next_state <= ERROR;
	end if;
	
	-- - - - - - - - - - - - - - - - - - - - - - -
	
	when ERROR =>
		next_state <= ERROR;
		if(KEY(15) = '1') then
			next_state <= PRINT_FALSE;
		elsif(KEY(15 downto 0) /= "0000000000000000") then
			next_state <= ERROR;
		end if;
			
   when PRINT_FALSE =>
      next_state <= PRINT_FALSE;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
		
	when PRINT_OK =>
		next_state <= PRINT_OK;
		if(CNT_OF = '1')then
			next_state <= FINISH;
		end if; 
			
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0'; --moor
   FSM_MX_MEM     <= '0'; --moor
   FSM_MX_LCD     <= '0'; --moor
   FSM_LCD_WR     <= '0'; --mealy
   FSM_LCD_CLR    <= '0'; --mealy

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_FALSE =>
		FSM_MX_MEM		<= '0';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
		
	when PRINT_OK => 
		FSM_MX_MEM		<= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
	   if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
		
   end case;
end process output_logic;

end architecture behavioral;

