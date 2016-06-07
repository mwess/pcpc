/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.apache.hadoop.examples;

import java.io.IOException;
import java.util.StringTokenizer;
import java.util.*;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;


import java.util.concurrent.*;


public class WordCount {

  public static class TokenizerMapper 
       extends Mapper<Object, Text, Text, Text>{
    
//    private final static IntWritable one = new IntWritable(1);
    private Text word = new Text();
    private Text key = new Text();
    private Text letter = new Text();
    private String[] str,lines;
    private String str2;
      
    public void map(Object key2, Text value, Context context
                    ) throws IOException, InterruptedException {
      lines = value.toString().split("\n");
      for(String line : lines){
          str = line.split("\t");
          key.set(str[0]);
          word.set(str[1]);
          context.write(key,word);
          context.write(word,key);
      }
    }
  }
  
  public static class AnagramReducer 
       extends Reducer<Text,Text,Text,Text> {
    private Text result = new Text();
    public void reduce(Text key, Iterable<Text> values, 
                       Context context
                       ) throws IOException, InterruptedException {
	ConcurrentSkipListSet<String>  h = new ConcurrentSkipListSet<String>();
      String str = "";
      for (Text val : values) {
	h.add(val.toString());
      }
      str = h.toString();
      str = str.replace("[","");
      str = str.replace("]","");
      str = str.replace(","," ");
      result.set(str);
      context.write(key, result);
    }
  }

  public static class TokenizerMapper2 
       extends Mapper<Object, Text, Text, Text>{
    
    private Text word = new Text();
    private Text key = new Text();
    private Text letter = new Text();
      
    public void map(Object key2, Text value, Context context
                    ) throws IOException, InterruptedException {
    String str,tmpstr,tmp;
    String str1,kstr,k;
    String[] lines;
      lines = value.toString().split("\n");
      for(String line : lines){
      
	      StringTokenizer itr = new StringTokenizer(line);

	      str1 = line.split("\t",2)[1];
	      k = itr.nextToken();
	      while(itr.hasMoreTokens()){
  		  str = itr.nextToken();
 		  tmpstr = k;
        	  if(str.compareTo(tmpstr)>0){
        	      tmp = str;
        	      str = tmpstr;
        	      tmpstr = tmp;
        	  }
	          kstr = str + " " + tmpstr;
		  key.set(kstr);
		  word.set(str1);
	      	  context.write(key,word);
	      }
}
    }
  }
  
  public static class AnagramReducer2 
       extends Reducer<Text,Text,Text,Text> {
    private Text result = new Text();
    public void reduce(Text key, Iterable<Text> values, 
                       Context context
                       ) throws IOException, InterruptedException {
    ArrayList<String> htmp = new ArrayList<String>();
    ArrayList<String> h = new ArrayList<String>();
    StringTokenizer itr;
    Iterator valuesitr;
    String s = "";
    valuesitr = values.iterator();
    itr = new StringTokenizer(valuesitr.next().toString());
    while(itr.hasMoreTokens()){
        h.add(itr.nextToken());
    }
    while(valuesitr.hasNext()){
        itr = new StringTokenizer(valuesitr.next().toString());
        htmp = new ArrayList<String>();
        while(itr.hasMoreTokens()){
            htmp.add(itr.nextToken());
        }
        h.retainAll(htmp);
    }
      result.set(h.toString());
      context.write(key, result);
    }
  }
  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
    if (otherArgs.length < 2) {
      System.err.println("Usage: wordcount <in> [<in>...] <out>");
      System.exit(2);
    }
    Job job = Job.getInstance(conf, "word count");
    job.setJarByClass(WordCount.class);
    job.setMapperClass(TokenizerMapper.class);
    job.setReducerClass(AnagramReducer.class);
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(Text.class);
    for (int i = 0; i < otherArgs.length - 1; ++i) {
      FileInputFormat.addInputPath(job, new Path(otherArgs[i]));
    }
    FileOutputFormat.setOutputPath(job,
      new Path(otherArgs[otherArgs.length - 1]));

    job.waitForCompletion(true);
    Job job2 = Job.getInstance(conf, "word count");
    job2.setJarByClass(WordCount.class);
    job2.setMapperClass(TokenizerMapper2.class);
    job2.setReducerClass(AnagramReducer2.class);
    job2.setOutputKeyClass(Text.class);
    job2.setOutputValueClass(Text.class);
    for (int i = 0; i < otherArgs.length - 1; ++i) {
      FileInputFormat.addInputPath(job2, new Path(otherArgs[otherArgs.length -1]+"/part-r-00000"));
    }
    FileOutputFormat.setOutputPath(job2,
      new Path(otherArgs[otherArgs.length - 1]+"2"));
    System.exit(job2.waitForCompletion(true) ? 0 : 1);
  }
}
