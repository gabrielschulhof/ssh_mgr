echo "echo -en '1234567890\r\033[5C\033[42m'; read; echo -en '\033[1P' ; echo -en '\033[m' ; read"; read
echo -en '1234567890\r\033[5C\033[42m'; read; echo -en '\033[1P' ; echo -en '\033[m' ; read
echo "echo -en '1234567890\r\033[5C\033[42m'; read; echo -en '\033[599P' ; echo -en '\033[m' ; read"; read
echo -en '1234567890\r\033[5C\033[42m'; read; echo -en '\033[599P' ; echo -en '\033[m' ; read
echo "echo -en '1234567890\r\033[5C\033[42m'; read; echo -en '\033[999P' ; echo -en '\033[m' ; read"; read
echo -en '1234567890\r\033[5C\033[42m'; read; echo -en '\033[999P' ; echo -en '\033[m' ; read
echo "echo -en '\033[1;37mxxx\033[0m'; read"; read
echo -en '\033[1;37mxxx\033[0m'; read
echo "echo -en '\033[1;37xxx\033[0m'; read"; read
echo -en '\033[1;37xxx\033[0m'; read
echo "echo -en '\033[2J\033[1;1H'; read"; read
echo -en '\033[2J\033[1;1H'; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m ' ; read "; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m ' ; read 
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[17;15H' ; read; echo -en '\033M' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[17;15H' ; read; echo -en '\033M' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[1;15H' ; read; echo -en '\033M' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[1;15H' ; read; echo -en '\033M' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[10;15H' ; read; echo -en '\033M' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[10;15H' ; read; echo -en '\033M' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[17;15H' ; read; echo -en '\033M' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[17;15H' ; read; echo -en '\033M' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;15H' ; read; echo -en '\033M' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;15H' ; read; echo -en '\033M' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;1H' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;1H' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;1H' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;1H' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;1H' ; read; echo -en '\0332M' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;1H' ; read; echo -en '\0332M' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;1H' ; read; echo -en '\033M' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[41m\033[8;17r\033[8;1H' ; read; echo -en '\033M' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;15r\033[41m\033[5;15H' ; read; echo -en '\033[1T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;15r\033[41m\033[5;15H' ; read; echo -en '\033[1T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;15r\033[41m\033[5;15H' ; read; echo -en '\033[9T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;15r\033[41m\033[5;15H' ; read; echo -en '\033[9T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3L\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3L\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3T\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3T\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3T\033[0m\033[1;25r' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3T\033[0m\033[1;25r' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[19;15H' ; read; echo -en '\033M\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[19;15H' ; read; echo -en '\033M\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[1;15H' ; read; echo -en '\033M' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[1;15H' ; read; echo -en '\033M' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[1;15H' ; read; echo -en '\033M\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[1;15H' ; read; echo -en '\033M\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[3;15H' ; read; echo -en '\033M\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[3;15H' ; read; echo -en '\033M\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[8;15H' ; read; echo -en '\033[3T\033[0m\033[1;25r' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[8;15H' ; read; echo -en '\033[3T\033[0m\033[1;25r' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;23r\033[41m\033[5;15H' ; read; echo -en '\033[9T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;23r\033[41m\033[5;15H' ; read; echo -en '\033[9T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;23r\033[41m\033[8;15H' ; read; echo -en '\033[9T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;23r\033[41m\033[8;15H' ; read; echo -en '\033[9T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;8r\033[41m\033[5;15H' ; read; echo -en '\033[1T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;8r\033[41m\033[5;15H' ; read; echo -en '\033[1T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;8r\033[41m\033[5;15H' ; read; echo -en '\033[9T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;8r\033[41m\033[5;15H' ; read; echo -en '\033[9T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;8r\033[41m\033[8;15H' ; read; echo -en '\033M\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;8r\033[41m\033[8;15H' ; read; echo -en '\033M\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;9r\033[41m\033[5;15H' ; read; echo -en '\033[1T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;9r\033[41m\033[5;15H' ; read; echo -en '\033[1T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;9r\033[41m\033[5;15H' ; read; echo -en '\033[9T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;9r\033[41m\033[5;15H' ; read; echo -en '\033[9T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;9r\033[41m\033[5;15H' ; read; echo -en '\033[T\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;9r\033[41m\033[5;15H' ; read; echo -en '\033[T\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;9r\033[41m\033[8;15H' ; read; echo -en '\033M\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;9r\033[41m\033[8;15H' ; read; echo -en '\033M\033[0m' ; read
echo "echo -en '\033[41m\033[2J'; read"; read
echo -en '\033[41m\033[2J'; read
echo "echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;10H'; read"; read
echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;10H'; read
echo "echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;1H\033[4L'; read"; read
echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;1H\033[4L'; read
echo "echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;1H\033[8;18r\033[4L'; read"; read
echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;1H\033[8;18r\033[4L'; read
echo "echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;1H\033[8;18r\033[4L'; read"; read
echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;1H\033[8;18r\033[4L'; read
echo "echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n\10\n\11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;10H'; read"; read
echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n\10\n\11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;10H'; read
echo "echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n\9\n\10\n\11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;10H'; read"; read
echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n\9\n\10\n\11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;10H'; read
echo "echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n\9n\10\n\11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;10H'; read"; read
echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n\9n\10\n\11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;10H'; read
echo "echo -en '\033[70C1234567890'; read; echo -en '\033[1P' ; echo -en '\033[m' ; read"; read
echo -en '\033[70C1234567890'; read; echo -en '\033[1P' ; echo -en '\033[m' ; read
echo "echo -en '\033[70C1234567890\r\033[42m'; read; echo -en '\033[1P' ; echo -en '\033[m' ; read"; read
echo -en '\033[70C1234567890\r\033[42m'; read; echo -en '\033[1P' ; echo -en '\033[m' ; read
echo "echo -en '\033[70C1234567890\r\033[5C\033[42m'; read; echo -en '\033[1P' ; echo -en '\033[m' ; read"; read
echo -en '\033[70C1234567890\r\033[5C\033[42m'; read; echo -en '\033[1P' ; echo -en '\033[m' ; read
echo "echo -en '\033[?25h'; read"; read
echo -en '\033[?25h'; read
echo "echo -en '\033[?25l'; read"; read
echo -en '\033[?25l'; read
echo "echo -en '\n   \033(0f\033(B   \n'; read"; read
echo -en '\n   \033(0f\033(B   \n'; read
echo "echo -en '\n   \033(Af\033(B   \n'; read"; read
echo -en '\n   \033(Af\033(B   \n'; read
echo "echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;1H\033[8;18r\033[4L'; read"; read
echo -en '\033[41m\033[2J1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[12;1H\033[8;18r\033[4L'; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3L\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3L\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3M\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3M\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3T\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[12;15H' ; read; echo -en '\033[3T\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[18;15H' ; read; echo -en '\033M\033[0m' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[18;15H' ; read; echo -en '\033M\033[0m' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[20;15H' ; read; echo -en '\033[3L\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[20;15H' ; read; echo -en '\033[3L\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[20;15H' ; read; echo -en '\033[3M\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[20;15H' ; read; echo -en '\033[3M\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[20;15H' ; read; echo -en '\033[3S\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[20;15H' ; read; echo -en '\033[3S\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[20;15H' ; read; echo -en '\033[3T\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[20;15H' ; read; echo -en '\033[3T\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[4;15H' ; read; echo -en '\033[3M\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[4;15H' ; read; echo -en '\033[3M\033[0m\033' ; read
echo "echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[4;15H' ; read; echo -en '\033[3S\033[0m\033' ; read"; read
echo -en '\033[2J\033[1;1H1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\033[8;17r\033[41m\033[4;15H' ; read; echo -en '\033[3S\033[0m\033' ; read
