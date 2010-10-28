/*BasePVStructureArray.h*/
#ifndef BASEPVSTRUCTUREARRAY_H
#define BASEPVSTRUCTUREARRAY_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    PVStructureArray::PVStructureArray(PVStructure *parent,
        StructureArrayConstPtr structureArray)
    : PVArray(parent,structureArray)
    {}

    PVStructureArray::~PVStructureArray() {}

    class BasePVStructureArray : public PVStructureArray {
    public:
        BasePVStructureArray(PVStructure *parent,
             StructureArrayConstPtr structureArray);
        virtual ~BasePVStructureArray();
        virtual StructureArrayConstPtr getStructureArray();
        virtual void setCapacity(int capacity);
        virtual int get(int offset, int length,
            StructureArrayData *data);
        virtual int put(int offset,int length,
            PVStructurePtrArray from, int fromOffset);
        virtual void toString(StringBuilder buf) ;
        virtual void toString(StringBuilder buf,int indentLevel) ;
        virtual bool operator==(PVField *pv);
        virtual bool operator!=(PVField *pv);
        virtual void shareData( PVStructurePtrArray value,int capacity,int length);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher);
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count);
    private:
        StructureArrayConstPtr structureArray;
        StructureArrayData *structureArrayData;
        PVStructurePtrArray value;
    };


    BasePVStructureArray::BasePVStructureArray(
        PVStructure *parent,StructureArrayConstPtr structureArray)
    : PVStructureArray(parent,structureArray),
      structureArray(structureArray),
      structureArrayData(new StructureArrayData()),
      value(new PVStructurePtr[0])
     {}

    BasePVStructureArray::~BasePVStructureArray()
    {
        delete structureArrayData;
        delete[] value;
    }

    void BasePVStructureArray::setCapacity(int capacity) {
        if(getCapacity()==capacity) return;
        if(!isCapacityMutable()) {
            std::string message("not capacityMutable");
            PVField::message(message, errorMessage);
            return;
        }
        int length = PVArray::getLength();
        PVStructurePtrArray  newValue = new PVStructurePtr[capacity];
        int limit = length;
        if(length>capacity) limit = capacity;
        for(int i=0; i<limit; i++) newValue[i] = value[i];
        for(int i=limit; i<capacity; i++) newValue[i] = 0; 
        if(length>capacity) length = capacity;
        delete[] value;
        value = newValue;
        setCapacityLength(capacity,length);
    }
    

    StructureArrayConstPtr BasePVStructureArray::getStructureArray() 
    {
        return structureArray;
    }

    int BasePVStructureArray::get(
        int offset, int len, StructureArrayData *data) 
    {
        int n = len;
        int length = getLength();
        if(offset+len > length) {
            n = length - offset;
            if(n<0) n = 0;
        }
        data->data = value;
        data->offset = offset;
        return n;
    }

    int BasePVStructureArray::put(int offset,int len,
        PVStructurePtrArray  from, int fromOffset)
    {
        if(isImmutable()) {
            message(String("field is immutable"), errorMessage);
            return 0;
        }
        if(from==value) return len;
        if(len<1) return 0;
        int length = getLength();
        int capacity = getCapacity();
        if(offset+len > length) {
            int newlength = offset + len;
            if(newlength>capacity) {
                setCapacity(newlength);
                newlength = capacity;
                len = newlength - offset;
                if(len<=0) return 0;
            }
            length = newlength;
            setLength(length);
        }
        StructureConstPtr structure = structureArray->getStructure();
        for(int i=0; i<len; i++) {
        	PVStructurePtr frompv = from[i+fromOffset];
        	if(frompv==0) {
        		value[i+offset] = 0;
        		continue;
        	}
        	if(frompv->getStructure()!=structure) {
                     throw std::invalid_argument(String(
                       "Element is not a compatible structure"));
        	}
        	value[i+offset] = frompv;
        }
        postPut();
        return len;      
    }

    void BasePVStructureArray::shareData(
        PVStructurePtrArray value,int capacity,int length)
    {
        this->value = value;
        setCapacity(capacity);
        setLength(length);
    }

    void BasePVStructureArray::toString(StringBuilder buf)  {toString(buf,0);}

    void BasePVStructureArray::toString(StringBuilder buf,int indentLevel) 
    {
       getConvert()->getString(buf,this,indentLevel);
       PVField::toString(buf,indentLevel);
    }

    void BasePVStructureArray::serialize(
        ByteBuffer *pbuffer,SerializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVStructureArray::deserialize(
        ByteBuffer *pbuffer,DeserializableControl *pflusher)
    {
        throw std::logic_error(notImplemented);
    }

    void BasePVStructureArray::serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count)
    {
        throw std::logic_error(notImplemented);
    }

    bool BasePVStructureArray::operator==(PVField  *pvField) 
    {
        return getConvert()->equals(this,pvField);
    }

    bool BasePVStructureArray::operator!=(PVField  *pvField) 
    {
        return !(getConvert()->equals(this,pvField));
    }

}}
#endif  /* BASEPVSTRUCTUREARRAY_H */