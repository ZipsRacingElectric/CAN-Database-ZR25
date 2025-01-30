// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_eeprom.h"

// C Standard Library
#include <stdio.h>

// Global Data ----------------------------------------------------------------------------------------------------------------

#define VCU_VARIABLE_COUNT (sizeof (vcuVariables) / sizeof (canEepromVariable_t))
static canEepromVariable_t vcuVariables [] =
{
	{
		.address = 0x10,
		.name = "APPS_1_MIN",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x12,
		.name = "APPS_1_MAX",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x14,
		.name = "APPS_2_MIN",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x16,
		.name = "APPS_2_MAX",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x18,
		.name = "BSE_F_MIN",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x1A,
		.name = "BSE_F_MAX",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x1C,
		.name = "BSE_R_MIN",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x1E,
		.name = "BSE_R_MAX",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x28,
		.name = "DRIVING_TORQUE_LIMIT",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x2C,
		.name = "DRIVING_TORQUE_BIAS",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x30,
		.name = "REGEN_TORQUE_LIMIT",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x34,
		.name = "REGEN_TORQUE_BIAS",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x38,
		.name = "TORQUE_ALGORITHM_INDEX",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x3C,
		.name = "POWER_LIMIT",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x40,
		.name = "POWER_LIMIT_PID_KP",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x44,
		.name = "POWER_LIMIT_PID_KI",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x48,
		.name = "POWER_LIMIT_PID_KD",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x4C,
		.name = "POWER_LIMIT_PID_A",
		.type = CAN_EEPROM_TYPE_FLOAT
	},
	{
		.address = 0x50,
		.name = "GLV_BATTERY_SAMPLE_11V5",
		.type = CAN_EEPROM_TYPE_UINT16_T
	},
	{
		.address = 0x52,
		.name = "GLV_BATTERY_SAMPLE_14V4",
		.type = CAN_EEPROM_TYPE_UINT16_T
	}
};

static canEeprom_t vcuEeprom =
{
	.name			= "VCU",
	.canAddress		= 0x750,
	.variables		= vcuVariables,
	.variableCount	= VCU_VARIABLE_COUNT
};

// Entrypoint -----------------------------------------------------------------------------------------------------------------

int main (int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf (stderr, "Format: can-eeprom-programmer <device name>\n");
		return -1;
	}

	const char* deviceName = argv [1];

	canSocket_t socket;
	if (!canSocketInit (&socket, deviceName))
	{
		fprintf (stderr, "Failed to open CAN socket '%s'.\n", deviceName);
		return -1;
	}

	if (!canSocketSetTimeout (&socket, 100))
	{
		fprintf (stderr, "Failed to set CAN socket timeout.\n");
		return -1;
	}

	while (true)
	{
		char selection;
		printf ("Enter an option:\n");
		printf (" w - Write to the EEPROM.\n");
		printf (" r - Read from the EEPROM.\n");
		printf (" m - Print a map of the EEPROM's memory.\n");
		printf (" v - Validate the EEPROM.\n");
		printf (" i - Invalidate the EEPROM.\n");
		printf (" c - Check the EEPROM's validity.\n");
		printf (" q - Quit the program.\n");

		uint16_t index;
		uint8_t data [4];

		fscanf (stdin, "%c%*1[\n]", &selection);
		switch (selection)
		{
		case 'w':
			index = canEepromVariableIndexPrompt (&vcuEeprom);
			canEepromVariableValuePrompt (&vcuEeprom, index, data);
			canEepromWrite (&vcuEeprom, &socket, index, data);
			break;
		case 'r':
			index = canEepromVariableIndexPrompt (&vcuEeprom);
			if (canEepromRead (&vcuEeprom, &socket, index, data))
				canEepromPrintVariable (&vcuEeprom, index, data);
			break;
		case 'm':
			canEepromPrintMap (&vcuEeprom, &socket);
			break;
		case 'v':
			canEepromValidate (&vcuEeprom, &socket, true);
			break;
		case 'i':
			canEepromValidate (&vcuEeprom, &socket, false);
			break;
		case 'c':
			bool isValid;
			if (canEepromIsValid (&vcuEeprom, &socket, &isValid))
				printf ("%s.\n", isValid ? "Valid" : "Invalid");
			break;
		case 'q':
			return 0;
		}
	};
}