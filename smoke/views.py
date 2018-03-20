# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from .models import smoke
import datetime
from django.shortcuts import render
from django.shortcuts import HttpResponse
# Create your views here.
def index(request):
    lastest_record=smoke.objects.order_by('-id')[0]
    content={'lastest_record':lastest_record}
    return render(request,'smoke/index.html',content)
def upload(request,value,longitude,latitude):
    info={"value":value,'longitude':longitude,'latitude':latitude}
    if(smoke.objects.create(**info)):
        return HttpResponse("Upload success")


