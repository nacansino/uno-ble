#!/bin/bash
#this script notifies the parent repository to build too when this is built

#go here https://docs.travis-ci.com/user/triggering-builds/

body='{
"request": {
"branch":"master"
}}'

curl -s -X POST -H "Content-Type: application/json" \
-H "Accept: application/json" -H "Travis-API-Version: 3" \
-H "Authorization: token ${TRAVIS_TOKEN}" \
-d '{"request":{"branch":"master"}}' \
-d "$body" \
https://api.travis-ci.com/repo/nacansino%2FRaspberry-Pi-as-Bluetooth-Hub/requests