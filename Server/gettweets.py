#!/usr/bin/env python

import time
from textwrap import TextWrapper

import tweepy

def main():
    consumer_key = 'XXX'
    consumer_secret = 'XXX'
    access_token = 'XXX'
    access_token_secret = 'XXX'

    auth = tweepy.auth.OAuthHandler(consumer_key, consumer_secret)
    auth.set_access_token(access_token, access_token_secret)

    api = tweepy.API(auth)

    while True:
        bsidesTweet = tweepy.Cursor(api.search, q='#BSidesCbr').items(10)
        for tweet in bsidesTweet:
	    if not tweet.retweeted and not (tweet.text[0] == 'R' and tweet.text[1] == 'T'):
            	msg = '@%s: %s\n' % (tweet.user.screen_name, tweet.text)
            	print msg
	    	f = open('/var/www/html/BSidesTwitterStream.txt', 'w')
	    	f.write(msg)
	    	f.close()
	    	time.sleep(60)

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print '\nGoodbye!'

