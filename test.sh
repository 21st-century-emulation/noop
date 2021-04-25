docker build -q -t noop .
docker run --rm --name noop -d -p 8080:8080 noop

RESULT=`curl -s --header "Content-Type: application/json" \
  --request POST \
  --data '{"id":"abcd","opcode":0,"state":{"a":181,"b":0,"c":0,"d":0,"e":0,"h":0,"l":0,"flags":{"sign":false,"zero":false,"auxCarry":false,"parity":false,"carry":false},"programCounter":0,"stackPointer":0,"cycles":0,"interruptsEnabled":true}}' \
  http://localhost:8080/api/v1/execute`
EXPECTED='{"id":"abcd","opcode":0,"state":{"a":181,"b":0,"c":0,"d":0,"e":0,"h":0,"l":0,"flags":{"sign":false,"zero":false,"auxCarry":false,"parity":false,"carry":false},"programCounter":0,"stackPointer":0,"cycles":4,"interruptsEnabled":true}}'

docker kill noop

if [ "$RESULT" = "$EXPECTED" ]; then
    echo -e "\e[32mNOOP Test Pass \e[0m"
    exit 0
else
    echo -e "\e[31mNOOP Test Fail  \e[0m"
    echo "$RESULT"
    exit -1
fi