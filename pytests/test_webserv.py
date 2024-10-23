# from copy import copy

#
# import msgspec
# from litestar import Litestar
# from litestar.status_codes import (
#     HTTP_200_OK,
#     HTTP_201_CREATED,
#     HTTP_204_NO_CONTENT,
#     HTTP_404_NOT_FOUND,
#     HTTP_409_CONFLICT,
# )
# from litestar.testing import AsyncTestClient
# from polyfactory.factories import DataclassFactory
#
# from webapp.models.customer import Customer
#
#
# class CustomerFactory(DataclassFactory[Customer]):
#     @classmethod
#     def name(cls) -> str:
#         return cls.__faker__.name()
#
#     @classmethod
#     def email(cls) -> str:
#         return cls.__faker__.email()
#
#     @classmethod
#     def address(cls) -> str:
#         return cls.__faker__.street_address()
#
#     @classmethod
#     def phonenumber(cls) -> str:
#         return cls.__faker__.basic_phone_number()
#
#
# # TODO Use a proper serializer thing from litestar
# def serialize_customer(json: dict[str, str]) -> Customer:
#     # test if json has the `Customer` fields, if no throw error.
#     for attr_name in Customer.__mapper__.attrs.keys():  # noqa: SIM118
#         if attr_name not in json:
#             raise KeyError(attr_name)
#     # TODO find a way to dynamically iterate over all the fields and get their type using `type`.
#     return Customer(
#         id=int(json["id"]),
#         name=json["name"],
#         address=json["address"],
#         email=json["email"],
#         phonenumber=json["phonenumber"],
#     )
#
#
# """
# Creates a `Customer` and POST it to the db
# Also checks if all the fields in the reponse are matching the input `Customer`
# """
#
#
# async def create_customer(test_client: AsyncTestClient[Litestar], customer: Customer | None = None) -> Customer:
#     if customer is None:
#         customer = CustomerFactory.build(id=None)
#
#     response = await test_client.post(url="/customer", content=msgspec.json.encode(customer))
#
#     assert response.status_code == HTTP_201_CREATED, response.text
#     response_json = response.json()
#
#     # Make sure there is a `id` created.
#     assert response_json["id"] is not None
#
#     # Iterate over all `Customer` fields and assert they are matching the POST results.
#     for attr in customer.__mapper__.attrs.keys():  # noqa: SIM118
#         # Skip the `Customer.id` since that is set to None when doing a POST.
#         if attr != "id":
#             assert response_json[attr] == getattr(customer, attr)
#
#     return serialize_customer(response_json)
#
#
# async def test_create_customer(test_client: AsyncTestClient[Litestar]) -> None:
#     await create_customer(test_client)
#
#
# async def test_create_customer_already_existing(test_client: AsyncTestClient[Litestar]) -> None:
#     # Create initial `Customer`
#     customer = await create_customer(test_client)
#     # Creation of second `Customer` which should fail
#     response = await test_client.post(url="/customer", content=msgspec.json.encode(customer))
#     assert response.status_code == HTTP_409_CONFLICT, response.text
#
#
# async def test_get_customer_by_id(test_client: AsyncTestClient[Litestar]) -> None:
#     customer_input = await create_customer(test_client)
#
#     # Run a `GET` request on the newly created user
#     response_get = await test_client.get(url=f"/customer/{customer_input.id}")
#
#     assert response_get.status_code == HTTP_200_OK, response_get.text
#     customer_response = serialize_customer(response_get.json())
#
#     # Should be the first index
#     assert customer_response.id == 1
#     assert customer_response == customer_input
#
#
# async def test_get_customer_non_existing(test_client: AsyncTestClient[Litestar]) -> None:
#     customer_id = 1
#     response_get = await test_client.get(url=f"/customer/{customer_id}")
#
#     assert response_get.status_code == HTTP_404_NOT_FOUND, response_get.text
#
#
# async def test_delete_customer(test_client: AsyncTestClient[Litestar]) -> None:
#     customer = await create_customer(test_client)
#
#     response = await test_client.delete(url=f"/customer/{customer.id}")
#     assert response.status_code == HTTP_204_NO_CONTENT, response.text
#
#
# async def test_delete_customer_non_existing(test_client: AsyncTestClient[Litestar]) -> None:
#     response = await test_client.delete(url=f"/customer/{1}")
#     assert response.status_code == HTTP_404_NOT_FOUND, response.text
#
#
# async def test_update_customer(test_client: AsyncTestClient[Litestar]) -> None:
#     customer = await create_customer(
#         test_client,
#         customer=Customer(id=None, name="Json", email="sjon@gmail.com", phonenumber="123", address="zegstraat"),
#     )
#
#     customer_new = copy(customer)
#     customer_new.name = "sjaak"
#
#     response = await test_client.put(url=f"/customer/{customer.id}", content=msgspec.json.encode(customer_new))
#     assert response.status_code == HTTP_200_OK, response.text
#
#     # explicitly assert `Customer.name`
#     assert customer.name != customer_new.name
#
#     # make sure all the other fields keep their value
#     for attr in customer.__mapper__.attrs.keys():  # noqa: SIM118
#         if attr != "name":
#             assert getattr(customer_new, attr) == getattr(customer, attr)
#
#
# async def test_update_customer_non_existing(test_client: AsyncTestClient[Litestar]) -> None:
#     customer = CustomerFactory.build(id=None)
#
#     response = await test_client.put(url=f"/customer/{customer.id}", content=msgspec.json.encode(customer))
#     assert response.status_code == HTTP_404_NOT_FOUND, response.text

# content of test_sample.py

from conftest import WebservConfig
from time import sleep

def test_webserv_instance(webserv_instance: int) -> None:
    print(f"pid: {webserv_instance.pid}")
    sleep(10)
