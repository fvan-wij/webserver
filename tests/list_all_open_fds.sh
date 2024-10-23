#!/bin/bash
ls -la /proc/$(pidof app)/fd 
# ls -la /proc/$(pidof app)/fd | grep -E 'socket|pipe'
