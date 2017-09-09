/*
 * SerializeHelper.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef SERIALIZEHELPER_H_
#define SERIALIZEHELPER_H_

class BinaryComSerializer;
class ServiceMessageBase;

void serializeMessage(const ServiceMessageBase& message, BinaryComSerializer& serializer);

#endif /* SERIALIZEHELPER_H_ */
