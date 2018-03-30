# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models
from datetime import  datetime
# Create your models here.
class smoke(models.Model):
    uploadtime=models.DateTimeField(default=datetime.now())
    value=models.TextField(default='-1')
    #value=models.FloatField(default=-1)
    #longitude=models.FloatField(default=116)
    longitude=models.TextField(default='116')
    latitude=models.TextField(default='40')
    #latitude=models.FloatField(default=40)
    def __unicode__(self):
       return self.value