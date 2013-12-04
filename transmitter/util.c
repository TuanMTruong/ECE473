/******************************************************************/
// Author: Tuan Truong
// Utility functions
//
/******************************************************************/




//pop buffer
uint8_t pop_buffer(uint8_t *buffer, uint8_t buffer_location){
	return buffer[buffer_location];
}

void push_buffer(uint8_t *buffer, uint8_t buffer_location, uint8_t data){
	*(buffer + buffer_location) = data;
	return;
}


//return 1 if there is a different in the two buffer
uint8_t compare_buffer(uint8_t *buffer1, uint8_t *buffer2){
	uint8_t i =0;
    
	while (*buffer1) {
		if (*buffer1 != *buffer2) {
			return 1;
		}
	}
	return 0;
    
}
