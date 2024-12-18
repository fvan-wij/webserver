#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Usage: $0 <number_of_concurrent_requests>"
  exit 1
fi

CONCURRENT_REQUESTS=$1

URL="http://localhost:9090" 

check_request() {
  URL=$1
  RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" $URL)
  if [ "$RESPONSE" -eq 200 ]; then
    echo "Request to $URL succeeded."
  else
    echo "Request to $URL failed with status code $RESPONSE."
  fi
}

export -f check_request

seq $CONCURRENT_REQUESTS | xargs -n 1 -P $CONCURRENT_REQUESTS bash -c "check_request $URL"
