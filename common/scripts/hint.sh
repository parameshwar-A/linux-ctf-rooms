#!/usr/bin/env bash
# Linux CTF - In-Container Hint Command

C_RESET="\033[0m"
C_CYAN="\033[1;36m"
C_YELLOW="\033[1;33m"
C_WHITE="\033[1;37m"

ROOM_FILE="/etc/linux_ctf_room.env"
if [ -f "$ROOM_FILE" ]; then
    source "$ROOM_FILE"
fi

ROOM_ID="${ROOM_ID:-01}"
ROOM_TITLE="${ROOM_TITLE:-Current Room}"
ROOM_HINT="${ROOM_HINT:-No hint available for this room.}"

echo -e "\n${C_CYAN}======================================================================${C_RESET}"
echo -e "  ${C_WHITE}[ HINT - LEVEL ${ROOM_ID} ]: ${ROOM_TITLE}${C_RESET}"
echo -e "${C_CYAN}======================================================================${C_RESET}"
echo -e "  ${C_YELLOW}${ROOM_HINT}${C_RESET}"
echo -e "${C_CYAN}======================================================================${C_RESET}\n"
